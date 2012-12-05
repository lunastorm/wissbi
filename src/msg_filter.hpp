#ifndef WISSBI_MSG_FILTER_HPP_
#define WISSBI_MSG_FILTER_HPP_

#include "msg_buf.hpp"

namespace wissbi {

template
<
    class input_policy,
    class output_policy
>
class MsgFilter : public input_policy, public output_policy
{
    using input_policy::Get;
    using output_policy::Put;
    typedef std::function<bool(MsgBuf& msg)> FuncType;

    public:
    MsgFilter() : filter_func_([](MsgBuf& msg){return true;}) {}

    bool Filter() {
        return Get(&msg_buf_) && filter_func_(msg_buf_) && Put(msg_buf_);
    }

    void FilterLoop() {
        while(Filter()) {}
    }

    void set_filter_func(FuncType filter_func) {
        filter_func_ = filter_func;
    }

    private:
    MsgBuf msg_buf_;
    FuncType filter_func_;
};

}

#endif  // WISSBI_MSG_FILTER_HPP_
