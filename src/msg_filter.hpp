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

    public:
    bool Filter() {
        return Get(&msg_buf_) && Put(msg_buf_);
    }

    void FilterLoop() {
        while(Filter()) {}
    }

    private:
    MsgBuf msg_buf_;
};

}

#endif  // WISSBI_MSG_FILTER_HPP_
