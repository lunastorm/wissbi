#ifndef WISSBI_BLOCK_POLICY_LINE_HPP_
#define WISSBI_BLOCK_POLICY_LINE_HPP_

#include "msg_buf.hpp"
#include <iosfwd>

namespace wissbi {
namespace block_policy {

class Line
{
    public:
    bool Read(std::istream& is, MsgBuf* msg_buf) {
        if(is.getline(msg_buf->buf, MAX_MSG_SIZE)) {
            msg_buf->len = is.gcount() - (is.eof() ? 0 : 1);
            return true;
        }
        else {
            msg_buf->len = 0;
            return false;
        }
    }
};

}
}

#endif  // WISSBI_BLOCK_POLICY_LINE_HPP_
