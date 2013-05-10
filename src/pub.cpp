#include <signal.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <unordered_set>
#include <atomic>
#include "connector.hpp"
#include "io_policy/line.hpp"
#include "io_policy/length.hpp"
#include "metric_reporter.hpp"
#include "logger.hpp"

using namespace wissbi;
using namespace std; 

void exit_signal_handler(int signum) {
}

void sleep_while(std::function<bool()> cond, int second) {
    auto wait_start_tp = chrono::system_clock::now();
    while(cond()) {
        if(second <= chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - wait_start_tp).count()) {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}

typedef MsgFilter<io_policy::Line, io_policy::Tee<io_policy::SysvMq>> StdLineInputFilter;
typedef MsgFilter<io_policy::Length, io_policy::Tee<io_policy::SysvMq>> StdLengthInputFilter;
typedef MsgFilter<io_policy::String, io_policy::Tee<io_policy::SysvMq>> MetricInputFilter;

template<class InputFilter>
void run(const std::string& dest, int wait_timeout_sec) {
    atomic_uint pending_counter(0);
    list<FilterMetric> metric_list;

    InputFilter input_filter;
    Connector<InputFilter> connector(dest, input_filter, metric_list, pending_counter);

    MetricInputFilter metric_input_filter;
    list<FilterMetric> dummy_metric_list;
    atomic_uint dummy_counter(0);
    Connector<MetricInputFilter> metric_connector("wissbi.metric", metric_input_filter, dummy_metric_list, dummy_counter);
    MetricReporter metric_reporter(metric_list, metric_input_filter, "enqueue");

    list<FilterMetric> drop_metric_list;
    drop_metric_list.push_back(FilterMetric(dest));
    MetricReporter drop_metric_reporter(drop_metric_list, metric_input_filter, "drop");

    thread([&metric_reporter, &drop_metric_reporter]{
        while(true) {
            this_thread::sleep_for(chrono::seconds(1));
            metric_reporter.Report();
            drop_metric_reporter.Report();
        }
    }).detach();

    input_filter.set_pre_filter_func([&input_filter, wait_timeout_sec] {
        sleep_while([&input_filter]{ return input_filter.GetBranchCount() == 0; }, wait_timeout_sec);
        return true;
    });
    input_filter.set_post_filter_func([dest, &input_filter, &pending_counter, &drop_metric_list](bool filter_result, MsgBuf& msgbuf){
        if(filter_result == true) {
            pending_counter += input_filter.GetLastTeeCount();
            drop_metric_list.begin()->last_processed += input_filter.GetLastFailedCount();
        }
        return filter_result;
    });

    input_filter.FilterLoop();

    sleep_while([dest, &pending_counter]{ return pending_counter > 0; }, wait_timeout_sec);

    metric_reporter.Report();
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        std::cerr << "Usage: " << argv[0] << " DESTINATION_NAME" << std::endl;
        return 1;
    }
    std::string dest(argv[1]);
    signal(SIGINT, exit_signal_handler);
    signal(SIGTERM, exit_signal_handler);
    signal(SIGPIPE, exit_signal_handler);

    if(getenv("WISSBI_PID_FILE") != NULL) {
        ofstream ofs(getenv("WISSBI_PID_FILE"));
        ofs << getpid() << endl;
    }

    int wait_timeout_sec = 1;
    char* wait_timeout_str = getenv("WISSBI_PUB_WAIT_TIMEOUT_SEC");
    if(wait_timeout_str) {
        istringstream iss(wait_timeout_str);
        iss >> wait_timeout_sec;
    }

    char* msg_format = getenv("WISSBI_MESSAGE_FORMAT");
    if(msg_format) {
        if(std::string(msg_format) == "line") {
            run<StdLineInputFilter>(dest, wait_timeout_sec);
        }
        else if(std::string(msg_format) == "length") {
            run<StdLengthInputFilter>(dest, wait_timeout_sec);
        }
        else {
            logger::log("unknown message format: {}", msg_format);
            return 1;
        }
    }
    else {
        run<StdLineInputFilter>(dest, wait_timeout_sec);
    }

	return 0;
}
