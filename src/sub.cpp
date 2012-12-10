#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <set>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/tcp.hpp"
#include "sub_entry.hpp"
#include "util.hpp"

using namespace std;
using namespace wissbi;

bool run = true;

void exit_signal_handler(int signum) {
    run = false;
}

int main(int argc, char* argv[]){
    signal(SIGINT, exit_signal_handler);
    signal(SIGTERM, exit_signal_handler);

    int serv = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr serv_addr;
    util::ConnectStringToSockaddr("0.0.0.0:0", reinterpret_cast<sockaddr_in*>(&serv_addr));
    if(-1 == ::bind(serv, &serv_addr, sizeof(serv_addr))) {
        cerr << "bind error" << endl;
        return -1;
    }
    if(-1 == listen(serv, 10)) {
        cerr << "listen error" << endl;
        return -1;
    }
    socklen_t len = sizeof(serv_addr);;
    getsockname(serv, &serv_addr, &len);
    cerr <<"after listen port "<<ntohs(((sockaddr_in*)&serv_addr)->sin_port)<<endl;

    ostringstream tmp;
    char buf[256];
    gethostname(buf, 256);
    struct addrinfo addr_hint;
    memset(&addr_hint, 0, sizeof(struct addrinfo));
    addr_hint.ai_family = AF_INET;
    addr_hint.ai_socktype = SOCK_STREAM;
    struct addrinfo *addr_head;
    getaddrinfo(buf, NULL, &addr_hint, &addr_head);
    tmp << inet_ntoa(((struct sockaddr_in *)(addr_head->ai_addr))->sin_addr) << ":" << ntohs(((sockaddr_in*)&serv_addr)->sin_port);
    
    SubEntry sub_entry(getenv("WISSBI_META_DIR") != NULL ? getenv("WISSBI_META_DIR") : "/var/lib/wissbi", argv[1], tmp.str());

    MsgFilter<io_policy::SysvMq, io_policy::Line> output_writer;
    thread* output_th = new thread([&output_writer](){
        output_writer.FilterLoop();
    });

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(serv, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  
    while(run) {
        sockaddr other_addr;
        socklen_t len;
        int res = accept(serv, &other_addr, &len);
        if(res > 0){
            cerr << "connected " << res << endl;
            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            setsockopt(res, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); 
            thread([res]{
                MsgFilter<io_policy::TCP, io_policy::SysvMq> filter;
                filter.set_cleanup(false);
                filter.AttachConnectedSock(res);
                filter.FilterLoop();
            }).detach();
        }
        sub_entry.renew();
    }

    return 0;
}

