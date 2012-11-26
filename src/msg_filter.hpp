#ifndef WISSBI_MSG_FILTER_HPP_
#define WISSBI_MSG_FILTER_HPP_

#include <iosfwd>

namespace wissbi {

template
<
    class input_policy
>
class MsgFilter : public input_policy
{
    using input_policy::Read;

    public:
    MsgFilter(std::istream& is) : is_(is) {}

    bool Filter(){
        return Read(is_, NULL);
    }

    private:
    std::istream& is_;
};

}

#endif  // WISSBI_MSG_FILTER_HPP_
