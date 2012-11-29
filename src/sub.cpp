#include <string.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <set>
#include <udt.h>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/udt.hpp"
#include "sub_entry.hpp"

using namespace std;
using namespace wissbi;

bool run = true;

void exit_signal_handler(int signum) {
    run = false;
}

int main(int argc, char* argv[]){
    signal(SIGINT, exit_signal_handler);
    signal(SIGTERM, exit_signal_handler);

    UDT::startup();
    
    UDTSOCKET serv = UDT::socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in my_addr;
    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(0);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(my_addr.sin_zero), '\0', 8);

    if (UDT::ERROR == UDT::bind(serv, (sockaddr*)&my_addr, sizeof(my_addr)))
    {
      cerr << "bind: " << UDT::getlasterror().getErrorMessage();
      return 0;
    }

    UDT::listen(serv, 10);
    int len;
    UDT::getsockname(serv, (sockaddr*)&my_addr, &len);
    cerr <<"after listen port "<<ntohs(my_addr.sin_port)<<endl;

    ostringstream tmp;
    tmp << "192.168.0.202:" << ntohs(my_addr.sin_port);

    
    SubEntry sub_entry("/var/lib/wissbi", argv[1], tmp.str());

    int namelen;
    sockaddr_in their_addr;

    bool is_blocking = false;
    cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_RCVSYN, &is_blocking, sizeof(bool)) << endl;
    int timeout = 1000;
    cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_RCVTIMEO, &timeout, sizeof(int)) << endl;

    //cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_SNDSYN, &is_nonblocking, sizeof(bool)) << endl;
    //cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_RCVTIMEO, &timeout, sizeof(int)) << endl;
    MsgFilter<io_policy::SysvMq, io_policy::Line> output_writer;
    thread* output_th = new thread([&output_writer](){
        output_writer.FilterLoop();
    });
  
    while(run) {
        int eid = UDT::epoll_create();
        UDT::epoll_add_usock(eid, serv);
        set<UDTSOCKET> readfds;
        set<UDTSOCKET> writefds;
        UDT::epoll_wait(eid, &readfds, &writefds, timeout);
//        cerr << "after wait" << endl;
        UDTSOCKET res = UDT::accept(serv, (sockaddr*)&their_addr, &namelen);
        if(res > 0){
            cerr << "connected " << res << endl;
            bool is_blocking = true;
    cerr << "setopt res: " << UDT::setsockopt(res, 0, UDT_RCVSYN, &is_blocking, sizeof(bool)) << endl;
    int timeout = -1;
    cerr << "setopt res: " << UDT::setsockopt(res, 0, UDT_RCVTIMEO, &timeout, sizeof(int)) << endl;
            thread([res]{
                MsgFilter<io_policy::Udt, io_policy::SysvMq> filter;
                filter.AttachConnectedSock(res);
                filter.FilterLoop();
            }).detach();
        }
        sub_entry.renew();
        UDT::epoll_release(eid);
    }

    return 0;
}

