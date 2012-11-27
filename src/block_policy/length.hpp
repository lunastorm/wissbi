#ifndef WISSBI_BLOCK_POLICY_LENGTH_HPP_
#define WISSBI_BLOCK_POLICY_LENGTH_HPP_

#include "msg_buf.hpp"
#include <iosfwd>

namespace wissbi {
namespace block_policy {

class Length
{
    public:
    bool Read(std::istream& is, MsgBuf* msg_buf) {
        int size;
        is >> size;
        if(!is.good()) {
            msg_buf->len = 0;
            return false;
        }
        is.get();
        is.read(msg_buf->buf, size);
        msg_buf->len = size;
        return true;
    }
};

}
}

#endif  // WISSBI_BLOCK_POLICY_LENGTH_HPP_
