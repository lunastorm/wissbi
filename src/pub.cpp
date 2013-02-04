#include <signal.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <unordered_set>
#include <atomic>
#include "msg_filter.hpp"
#include "sub_dir.hpp"
#include "io_policy/line.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/tcp.hpp"
#include "util.hpp"

using namespace wissbi;
using namespace std; 

atomic_uint in_process_cnt(0);

void exit_signal_handler(int signum) {
}

void scan_dest_loop(const string& dest) {
    unordered_set<string> producer_set;

    SubDir sub_dir(getenv("WISSBI_META_DIR") != NULL ? getenv("WISSBI_META_DIR") : "/var/lib/wissbi", dest);
    while(true) {
        for(auto conn_str : sub_dir.GetSubList()) {
            if(producer_set.find(conn_str) != producer_set.end()) {
                continue;
            }

            thread([conn_str]{
                sockaddr sock_addr;
                util::ConnectStringToSockaddr(conn_str, reinterpret_cast<sockaddr_in*>(&sock_addr));
                MsgFilter<io_policy::SysvMq, io_policy::TCP> producerFilter;
                producerFilter.set_post_filter_func([](MsgBuf& msg_buf){
                    in_process_cnt--;
                    return true;
                });
                producerFilter.Connect(&sock_addr);
                producerFilter.FilterLoop();
            }).detach();
            producer_set.insert(conn_str);
        }
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, exit_signal_handler);
    signal(SIGTERM, exit_signal_handler);

    thread(scan_dest_loop, argv[1]).detach();

    MsgFilter<io_policy::Line, io_policy::SysvMq> input_filter;
    input_filter.set_post_filter_func([](MsgBuf& msg_buf){
        in_process_cnt++;
        return true;
    });
    input_filter.FilterLoop();

    int wait_timeout_sec = 1;
    char* wait_timeout_str = getenv("WISSBI_PUB_WAIT_TIMEOUT_SEC");
    if(wait_timeout_str) {
        istringstream iss(wait_timeout_str);
        iss >> wait_timeout_sec;
    }

    auto wait_start_tp = chrono::system_clock::now();
    while(in_process_cnt) {
        if(wait_timeout_sec <= chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - wait_start_tp).count()) {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

	return 0;
}
