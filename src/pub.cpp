#include <signal.h>
#include <iostream>
#include <sstream>
#include <thread>
#include "msg_filter.hpp"
#include "sub_dir.hpp"
#include "io_policy/line.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/tcp.hpp"
#include "util.hpp"

using namespace wissbi;
using namespace std; 

void exit_signal_handler(int signum) {
}

int main(int argc, char* argv[]) {
    signal(SIGINT, exit_signal_handler);
    signal(SIGTERM, exit_signal_handler);

    SubDir sub_dir(getenv("WISSBI_META_DIR") != NULL ? getenv("WISSBI_META_DIR") : "/var/lib/wissbi", argv[1]);
    for(string conn_str : sub_dir.GetSubList()) {
        thread([conn_str]{
            sockaddr sock_addr;
            util::ConnectStringToSockaddr(conn_str, reinterpret_cast<sockaddr_in*>(&sock_addr));
            MsgFilter<io_policy::SysvMq, io_policy::TCP> producerFilter;
            producerFilter.Connect(&sock_addr);
            producerFilter.FilterLoop();
        }).detach();
    }

    MsgFilter<io_policy::Line, io_policy::SysvMq> input_filter;
    input_filter.FilterLoop();

    int wait_timeout_sec = -1;
    char* wait_timeout_str = getenv("WISSBI_PUB_WAIT_TIMEOUT_SEC");
    if(wait_timeout_str) {
        istringstream iss(wait_timeout_str);
        iss >> wait_timeout_sec;
    }

    auto wait_start_tp = chrono::system_clock::now();
    while(input_filter.GetCount() > 0) {
        if(wait_timeout_sec <= chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - wait_start_tp).count()) {
            break;
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }

	return 0;
}
