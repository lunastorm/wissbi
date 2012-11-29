#include <iostream>
#include <thread>
#include "msg_filter.hpp"
#include "sub_dir.hpp"
#include "io_policy/line.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/udt.hpp"
#include "util.hpp"

using namespace wissbi;
using namespace std; 

int main(int argc, char* argv[]) {
	UDT::startup();

    SubDir sub_dir("/var/lib/wissbi", argv[1]);
    for(string conn_str : sub_dir.GetSubList()) {
        cerr << "connstr: " << conn_str << endl;
        thread([conn_str]{
            sockaddr sock_addr;
            util::ConnectStringToSockaddr(conn_str, reinterpret_cast<sockaddr_in*>(&sock_addr));
            MsgFilter<io_policy::SysvMq, io_policy::Udt> producerFilter;
            producerFilter.Connect(&sock_addr);
            producerFilter.FilterLoop();
        }).detach();
    }

    MsgFilter<io_policy::Line, io_policy::SysvMq> input_filter;
    input_filter.FilterLoop();
	return 0;
}
