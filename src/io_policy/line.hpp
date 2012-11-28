#ifndef WISSBI_IO_POLICY_LINE_HPP_
#define WISSBI_IO_POLICY_LINE_HPP_

#include "msg_buf.hpp"
#include "io_policy/iostream_base.hpp"

namespace wissbi {
namespace io_policy {

class Line : public IOStreamBase
{
    public:
    bool Get(MsgBuf *msg_ptr) {
        if(is_ptr_->getline(msg_ptr->buf, MAX_MSG_SIZE)) {
            msg_ptr->len = is_ptr_->gcount() - (is_ptr_->eof() ? 0 : 1);
            return true;
        }
        else {
            msg_ptr->len = 0;
            return false;
        }
    }

    bool Put(const MsgBuf &msg) {
        os_ptr_->write(msg.buf, msg.len);
        os_ptr_->write("\n", 1);
        return true;
    }
};

}
}

#endif  // WISSBI_IO_POLICY_LINE_HPP_
