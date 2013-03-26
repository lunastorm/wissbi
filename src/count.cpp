#include <ctime>
#include <iostream>
#include "msg_filter.hpp"
#include "io_policy/line.hpp"

using namespace wissbi;
using namespace std;

int main(int argc, char* argv[]) {
    time_t last_update_ts = time(NULL);
    int count = 0;

    MsgFilter<io_policy::Line, io_policy::Line> counter;
    static_cast<OutputWrapper<io_policy::Line>&>(counter).auto_flush();
    counter.set_filter_func([&last_update_ts, &count](MsgBuf& msg){
        count++;
        time_t current_ts = time(NULL);
        if(current_ts > last_update_ts) {
            cerr.precision(3);
            cerr << fixed << (float)count / (current_ts - last_update_ts) << " messages / sec" << endl;
            last_update_ts = current_ts;
            count = 0;
        }
        return true;
    });
    counter.FilterLoop();
}
