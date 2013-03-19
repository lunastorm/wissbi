#include <signal.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <unordered_set>
#include <atomic>
#include "connector.hpp"
#include "io_policy/line.hpp"
#include "metric_reporter.hpp"

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

typedef MsgFilter<io_policy::Line, io_policy::Tee<io_policy::SysvMq>> StdInputFilter;
typedef MsgFilter<io_policy::String, io_policy::Tee<io_policy::SysvMq>> MetricInputFilter;

int main(int argc, char* argv[]) {
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

    list<FilterMetric> metric_list;
    atomic_uint pending_counter(0);

    StdInputFilter input_filter;
    Connector<StdInputFilter> connector(dest, input_filter, metric_list, pending_counter);

    MetricInputFilter metric_input_filter;
    list<FilterMetric> dummy_metric_list;
    atomic_uint dummy_counter(0);
    Connector<MetricInputFilter> metric_connector("wissbi.metric", metric_input_filter, dummy_metric_list, dummy_counter);
    MetricReporter metric_reporter(metric_list, metric_input_filter, "enqueue");

    thread([&metric_reporter]{
        while(true) {
            this_thread::sleep_for(chrono::seconds(1));
            metric_reporter.Report();
        }
    }).detach();

    input_filter.set_pre_filter_func([&input_filter, wait_timeout_sec] {
        sleep_while([&input_filter]{ return input_filter.GetBranchCount() == 0; }, wait_timeout_sec);
        return true;
    });
    input_filter.set_post_filter_func([dest, &input_filter, &pending_counter](bool filter_result, MsgBuf& msgbuf){
        if(filter_result == true) {
            pending_counter += input_filter.GetLastTeeCount();
        }
        return filter_result;
    });

    input_filter.FilterLoop();

    sleep_while([dest, &pending_counter]{ return pending_counter > 0; }, wait_timeout_sec);

    metric_reporter.Report();

	return 0;
}
