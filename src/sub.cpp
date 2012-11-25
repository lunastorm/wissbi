#include <string.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <set>
#include <udt.h>
#include "sub_entry.hpp"

using namespace std;
using namespace wissbi;

bool run = true;

void exit_signal_handler(int signum) {
    run = false;
}

typedef struct output_msg {
    long mtype;
    char buf[4001];
} output_msg;

void *receiver_loop(UDTSOCKET recver) {
    cerr << "in here recver: " << recver << endl;
    //recver_arg *arg_ptr = (recver_arg*)arg;
    //UDTSOCKET recver = arg_ptr->recver;

    output_msg msg;
    msg.mtype = 1;
    for(;;) {
//        cerr << "before recv " << recver << endl;
        int rsize = UDT::recvmsg(recver, msg.buf, 4000);
  //      cerr << "after recv rsize " <<rsize << endl;
        if(rsize == UDT::ERROR) {
            cerr << "recv error " << UDT::getlasterror().getErrorMessage() << endl;
            break;
        }
        msg.buf[rsize] = '\0';
        cout << "rsize: " << rsize << " content: " << msg.buf << endl;
        //int res = msgsnd(mqid, &msg, rsize + sizeof(long), 0);
        //cerr << "res: " << res << endl;
    }
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

    
    //SubEntry sub_entry("/var/lib/wissbi", argv[1], tmp.str());
    //thread heartbeat_th(heartbeat_loop, std::move(SubEntry("/var/lib/wissbi", argv[1], tmp.str())));
    SubEntry sub_entry("/var/lib/wissbi", argv[1], tmp.str());

    int namelen;
    sockaddr_in their_addr;

    bool is_blocking = false;
    cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_RCVSYN, &is_blocking, sizeof(bool)) << endl;
    int timeout = 1000;
    cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_RCVTIMEO, &timeout, sizeof(int)) << endl;

    //cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_SNDSYN, &is_nonblocking, sizeof(bool)) << endl;
    //cerr << "setopt res: " << UDT::setsockopt(serv, 0, UDT_RCVTIMEO, &timeout, sizeof(int)) << endl;
  
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
            thread* rcv_th = new thread(receiver_loop, res);
        }
        sub_entry.renew();
        UDT::epoll_release(eid);
/*
        recver_arg* arg = (recver_arg*)malloc(sizeof(recver_arg));
        arg->recver = recver;
        pthread_t t;
        pthread_create(&t, NULL, receiver_loop, arg);
*/
    }

    return 0;
}

