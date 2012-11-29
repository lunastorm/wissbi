#ifndef WISSBI_IO_POLICY_UDT_HPP_
#define WISSBI_IO_POLICY_UDT_HPP_

#include "msg_buf.hpp"
#include "udt.h"

namespace wissbi {
namespace io_policy {

class Udt {
    public:
    Udt() {
        sock_ = UDT::socket(AF_INET, SOCK_DGRAM, 0);
    }

    ~Udt() {
        UDT::close(sock_);
    }

    bool Put(const MsgBuf &msg) {
        if(msg.len <= 0) {
            return false;
        }
        int res = UDT::sendmsg(sock_, msg.buf, msg.len, 100);
        if(UDT::ERROR == res) {
            return false;
        }
        return true;
    }

    bool Get(MsgBuf *msg_ptr) {
        int res = UDT::recvmsg(sock_, msg_ptr->buf, msg_ptr->len);
        if(res <= 0) {
            msg_ptr->len = 0;
            return false;
        }
        msg_ptr->len = res;
        return true;
    }

    void Connect(sockaddr* addr) {
        if(UDT::ERROR == UDT::connect(sock_, addr, sizeof(*addr))) {
            std::cerr << "connect error: " << UDT::getlasterror().getErrorMessage() << std::endl;
            throw "connect error";
        }
    }

    private:
    UDTSOCKET sock_;
};

}
}

#endif  // WISSBI_IO_POLICY_UDT_HPP_
