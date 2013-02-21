#include <signal.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"

using namespace wissbi;
using namespace std;

deque<string> mq;

void dump_mq_and_exit() {
    char tmp[20] = "/tmp/wsbrec.XXXXXX";
    char *filename = mktemp(tmp);
    if(filename != NULL) {
        ofstream ofs(filename);
        for_each(mq.cbegin(), mq.cend(), [&ofs](const string& msg) {
            ofs << msg << endl;
        });
        cerr << "Messages dumped to " << filename << endl;
    }
}

void exit_signal_handler(int signum) {
    exit(signum);
}

int main(int argc, char* argv[]) {
    atexit(dump_mq_and_exit);
    signal(SIGPIPE, exit_signal_handler);

    int max_size = 50;

    MsgFilter<io_policy::Line, io_policy::Line> recorder;
    recorder.set_filter_func([&mq, max_size](MsgBuf& msg){
        if(mq.size() == max_size) {
            mq.pop_front();
        }
        mq.push_back(string(msg.buf, msg.len));
        return true;
    });
    recorder.FilterLoop();
}
