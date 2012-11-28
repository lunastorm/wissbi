#ifndef WISSBI_IO_POLICY_UDT_HPP_
#define WISSBI_IO_POLICY_UDT_HPP_

#include "msg_buf.hpp"
#include "udt.h"

namespace wissbi {
namespace io_policy {

class Udt {
    public:
    Udt() {
        //sock_ = UDT::socket(AF_INET, SOCK_DGRAM, 0);
    }

    ~Udt() {
        UDT::close(sock_);
    }

    bool Put(const MsgBuf &msg) {
        if(msg.len <= 0) {
            return false;
        }
        int res = UDT::sendmsg(sock_, msg.buf, msg.len, 100);
  std::cerr<<"res: " << res << std::endl;
        if(UDT::ERROR == res) {
            return false;
        }
        return true;
    }

    bool Get(MsgBuf *msg_ptr) {
        return false;
    }

    void Connect(sockaddr* addr) {
        sock_ = UDT::socket(AF_INET, SOCK_DGRAM, 0);
        if(UDT::ERROR == UDT::connect(sock_, addr, sizeof(*addr))) {
            std::cerr << "connect error: " << UDT::getlasterror().getErrorMessage() << std::endl;
            //std::cerr << inet_ntoa(saddr.sin_addr) <<":"<<ntohs(saddr.sin_port)<< std::endl;
        }
        std::cerr << "connected" << std::endl;
    }

    private:
    UDTSOCKET sock_;
};

}
}

#endif  // WISSBI_IO_POLICY_UDT_HPP_
