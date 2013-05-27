#include <assert.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <algorithm>
#include <tuple>
#include <chrono>
#include <mutex>
#include <thread>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"
#include "logger.hpp"

using namespace wissbi;
using namespace std;

mutex mq_mtx;
deque<tuple<string, long>> mq;
std::string filename;
char *filename_ptr = NULL;
bool running = true;

void dump_mq_and_exit() {
    char tmp[20] = "/tmp/wsbrec.XXXXXX";
    if(filename_ptr == NULL) {
        filename_ptr = mktemp(tmp);
    }
    assert(filename_ptr != NULL);

    ofstream ofs(filename_ptr);
    mq_mtx.lock();
    for_each(mq.cbegin(), mq.cend(), [&ofs](const tuple<string, long>& t) {
        ofs << std::get<0>(t) << endl;
    });
    mq_mtx.unlock();
    wissbi::logger::log("Messages dumped to {}", filename_ptr);
}

void exit_signal_handler(int signum) {
    running = false;
    exit(signum);
}

long currentTimeMillis() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void gc(long ttl) {
    while(running) {
        mq_mtx.lock();
            while(mq.size() && ttl < (currentTimeMillis() - std::get<1>(mq.front()))) {
                mq.pop_front();
            }
        mq_mtx.unlock();
        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    atexit(dump_mq_and_exit);
    signal(SIGINT, exit_signal_handler);
    signal(SIGPIPE, exit_signal_handler);
    if(argc > 1) {
        filename_ptr = argv[1];
    }

    int max_num = 50;
    char* max_num_str = getenv("WISSBI_RECORD_MAX_MSGNUM");
    if(max_num_str) {
        istringstream iss(max_num_str);
        iss >> max_num;
    }

    long ttl = 0;
    char* ttl_str = getenv("WISSBI_RECORD_TTL");
    if(ttl_str) {
        istringstream iss(ttl_str);
        iss >> ttl;
        ttl *= 1000;
        thread th(gc, ttl);
        th.detach();
    }

    MsgFilter<io_policy::Line, io_policy::Line> recorder;
    deque<tuple<string, long>> *mq_ptr = &mq;
    static_cast<OutputWrapper<io_policy::Line>&>(recorder).auto_flush();
    recorder.set_filter_func([mq_ptr, max_num](MsgBuf& msg){
        mq_mtx.lock();
        if(max_num && mq_ptr->size() == max_num) {
            mq_ptr->pop_front();
        }
        mq_ptr->push_back(forward_as_tuple(string(msg.buf, msg.len),
            currentTimeMillis()));
        mq_mtx.unlock();
        return true;
    });
    recorder.FilterLoop();
}
