#include <assert.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"
#include "logger.hpp"

using namespace wissbi;
using namespace std;

deque<string> mq;
std::string filename;
char *filename_ptr = NULL;

void dump_mq_and_exit() {
    char tmp[20] = "/tmp/wsbrec.XXXXXX";
    if(filename_ptr == NULL) {
        filename_ptr = mktemp(tmp);
    }
    assert(filename_ptr != NULL);

    ofstream ofs(filename_ptr);
    for_each(mq.cbegin(), mq.cend(), [&ofs](const string& msg) {
        ofs << msg << endl;
    });
    wissbi::logger::log("Messages dumped to {}", filename_ptr);
}

void exit_signal_handler(int signum) {
    exit(signum);
}

int main(int argc, char* argv[]) {
    atexit(dump_mq_and_exit);
    signal(SIGPIPE, exit_signal_handler);
    if(argc > 1) {
        filename_ptr = argv[1];
    }

    int max_size = 50;

    MsgFilter<io_policy::Line, io_policy::Line> recorder;
    deque<string> *mq_ptr = &mq;
    static_cast<OutputWrapper<io_policy::Line>&>(recorder).auto_flush();
    recorder.set_filter_func([mq_ptr, max_size](MsgBuf& msg){
        if(mq_ptr->size() == max_size) {
            mq_ptr->pop_front();
        }
        mq_ptr->push_back(string(msg.buf, msg.len));
        return true;
    });
    recorder.FilterLoop();
}
