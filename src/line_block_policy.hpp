#ifndef WISSBI_LINE_BLOCK_POLICY_HPP_
#define WISSBI_LINE_BLOCK_POLICY_HPP_

#include "msg_buf.hpp"
#include <iosfwd>

namespace wissbi {

class LineBlockPolicy
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

#endif  // WISSBI_LINE_BLOCK_POLICY_HPP_
