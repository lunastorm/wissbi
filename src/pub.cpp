#include <signal.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <unordered_set>
#include <atomic>
#include "msg_filter.hpp"
#include "sub_dir.hpp"
#include "io_policy/line.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/tcp.hpp"
#include "io_policy/tee.hpp"
#include "util.hpp"

using namespace wissbi;
using namespace std; 

atomic_uint in_process_cnt(0);

void exit_signal_handler(int signum) {
}

typedef MsgFilter<io_policy::Line, io_policy::Tee<io_policy::SysvMq>> InputFilter;

void scan_dest_loop(const string& dest, InputFilter& input_filter) {
    unordered_set<string> producer_set;

    SubDir sub_dir(getenv("WISSBI_META_DIR") != NULL ? getenv("WISSBI_META_DIR") : "/var/lib/wissbi", dest);
    while(true) {
        for(auto sub_entry_tuple : sub_dir.GetSubList()) {
            const std::string& conn_str = std::get<0>(sub_entry_tuple);
            const std::string& dest = std::get<1>(sub_entry_tuple);
            if(producer_set.find(conn_str) != producer_set.end()) {
                continue;
            }
            if(!input_filter.ExistsBranch(dest)) {
                shared_ptr<io_policy::SysvMq> mq_ptr(new io_policy::SysvMq());
                mq_ptr->mq_init(dest);
                input_filter.AddBranch(dest, mq_ptr);
            }

            thread([conn_str, dest, &producer_set]{
                sockaddr sock_addr;
                util::ConnectStringToSockaddr(conn_str, reinterpret_cast<sockaddr_in*>(&sock_addr));
                MsgFilter<io_policy::SysvMq, io_policy::TCP> producerFilter;
                try {
                    producerFilter.Connect(&sock_addr);
                }
                catch (...) {
                    cerr << "error connecting" << endl;
                    producer_set.erase(conn_str);
                    return;
                }
                producerFilter.set_post_filter_func([](MsgBuf& msg_buf){
                    in_process_cnt--;
                    return true;
                });
                producerFilter.mq_init(dest);
                producerFilter.FilterLoop();
                producer_set.erase(conn_str);
            }).detach();
            producer_set.insert(conn_str);
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
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

int main(int argc, char* argv[]) {
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

    InputFilter input_filter;
    thread(scan_dest_loop, argv[1], std::ref(input_filter)).detach();

    input_filter.set_pre_filter_func([&input_filter, wait_timeout_sec](MsgBuf& msgbuf){
        sleep_while([&input_filter]{ return input_filter.GetBranchCount() == 0; }, wait_timeout_sec);
        return true;
    });
    input_filter.set_post_filter_func([&input_filter](MsgBuf& msgbuf){
        in_process_cnt += input_filter.GetLastTeeCount();
        return true;
    });

    input_filter.FilterLoop();

    sleep_while([&in_process_cnt]{ return in_process_cnt > 0; }, wait_timeout_sec);

	return 0;
}
