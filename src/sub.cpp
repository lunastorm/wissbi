#include <string.h>
#include <signal.h>
#include <poll.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <set>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"
#include "io_policy/length.hpp"
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

template<class OutputWriter>
void run_sub(const std::string& src) {
    int serv = socket(AF_INET, SOCK_STREAM, 0);
    int opts;
    opts = fcntl(serv, F_GETFL);
    opts |= O_NONBLOCK;
    assert(0 == fcntl(serv, F_SETFL, opts));
    sockaddr serv_addr;
    util::ConnectStringToSockaddr("0.0.0.0:0", reinterpret_cast<sockaddr_in*>(&serv_addr));
    if(-1 == ::bind(serv, &serv_addr, sizeof(serv_addr))) {
        cerr << "bind error" << endl;
        throw std::runtime_error("bind error");
    }
    if(-1 == listen(serv, 10)) {
        cerr << "listen error" << endl;
        throw std::runtime_error("listen error");
    }
    socklen_t len = sizeof(serv_addr);;
    getsockname(serv, &serv_addr, &len);
    cerr <<"after listen port "<<ntohs(((sockaddr_in*)&serv_addr)->sin_port)<<endl;

    ostringstream tmp;
    tmp << util::GetHostIP() << ":" << ntohs(((sockaddr_in*)&serv_addr)->sin_port);
    
    SubEntry sub_entry(getenv("WISSBI_META_DIR") != NULL ? getenv("WISSBI_META_DIR") : "/var/lib/wissbi", src, tmp.str());

    OutputWriter output_writer;
    output_writer.mq_init("");
    output_writer.auto_flush();
    thread* output_th = new thread([&output_writer](){
        output_writer.FilterLoop();
    });

    struct pollfd fds[1];
    fds[0].fd = serv;
    fds[0].events = POLLIN;
  
    while(run) {
        if(poll(fds, 1, 1000) <= 0) {
            continue;
        }

        sockaddr other_addr;
        socklen_t len;
        int res = accept(serv, &other_addr, &len);
        if(res > 0){
            cerr << "connected " << res << endl;
            int opts;
            opts = fcntl(res, F_GETFL);
            opts ^= O_NONBLOCK;
            fcntl(res, F_SETFL, opts);

            struct timeval tv;
            tv.tv_sec = 0;
            tv.tv_usec = 0;
            setsockopt(res, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); 
            thread([res]{
                MsgFilter<io_policy::TCP, io_policy::SysvMq> filter;
                filter.mq_init("");
                filter.set_cleanup(false);
                filter.AttachConnectedSock(res);
                filter.FilterLoop();
            }).detach();
        }
        sub_entry.renew();
    }
}

int main(int argc, char* argv[]){
    signal(SIGINT, exit_signal_handler);
    signal(SIGTERM, exit_signal_handler);

    std::string src(argv[1]);

    if(getenv("WISSBI_PID_FILE") != NULL) {
        ofstream ofs(getenv("WISSBI_PID_FILE"));
        ofs << getpid() << endl;
    }

    char* msg_format = getenv("WISSBI_MESSAGE_FORMAT");
    if(msg_format) {
        if(std::string(msg_format) == "line") {
            run_sub<MsgFilter<io_policy::SysvMq, io_policy::Line>>(src);
        }
        else if(std::string(msg_format) == "length") {
            run_sub<MsgFilter<io_policy::SysvMq, io_policy::Length>>(src);
        }
        else {
            std::cerr << "unknown message format: " << msg_format << std::endl;
            return 1;
        }
    }
    else {
        run_sub<MsgFilter<io_policy::SysvMq, io_policy::Line>>(src);
    }

    return 0;
}

