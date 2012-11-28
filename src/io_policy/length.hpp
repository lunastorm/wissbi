#ifndef WISSBI_IO_POLICY_LENGTH_HPP_
#define WISSBI_IO_POLICY_LENGTH_HPP_

#include "msg_buf.hpp"
#include "io_policy/iostream_base.hpp"

namespace wissbi {
namespace io_policy {

class Length : public IOStreamBase
{
    public:
    bool Get(MsgBuf *msg_ptr) {
        int size;
        (*is_ptr_) >> size;
        if(!is_ptr_->good()) {
            msg_ptr->len = 0;
            return false;
        }
        is_ptr_->get();
        is_ptr_->read(msg_ptr->buf, size);
        msg_ptr->len = size;
        return true;
    }

    bool Put(const MsgBuf &msg) {
        if(msg.len <= 0) {
            return false;
        }
        (*os_ptr_) << msg.len << " ";
        os_ptr_->write(msg.buf, msg.len);
        return true;
    }
};

}
}

#endif  // WISSBI_IO_POLICY_LENGTH_HPP_
