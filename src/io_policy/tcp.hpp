#ifndef WISSBI_IO_POLICY_TCP_HPP_
#define WISSBI_IO_POLICY_TCP_HPP_

#include "msg_buf.hpp"
#include "util.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdexcept>

namespace wissbi {
namespace io_policy {

class TCP {
    public:
    TCP() {
        sock_ = socket(AF_INET, SOCK_STREAM, 0);
    }

    ~TCP() {
        close(sock_);
    }

    bool Put(const MsgBuf &msg) {
        if(msg.len <= 0) {
            return false;
        }
        uint32_t len_header = htonl(msg.len);
        return WriteAll(sock_, reinterpret_cast<char*>(&len_header), 4) && WriteAll(sock_, msg.buf, msg.len);
    }

    bool Get(MsgBuf *msg_ptr) {
        uint32_t len_header;
        if(4 != ReadAll(sock_, reinterpret_cast<char*>(&len_header), 4)) {
            msg_ptr->len = 0;
            return false;
        }
        msg_ptr->len = ntohl(len_header);
        if(msg_ptr->len != ReadAll(sock_, msg_ptr->buf, msg_ptr->len)) {
            msg_ptr->len = 0;
            return false;
        }
        return true;
    }

    void Connect(const sockaddr *addr) {
        if(-1 == connect(sock_, addr, sizeof(*addr))) {
            throw std::runtime_error(std::string("cannot connect to ") + util::SockaddrToConnectString(*reinterpret_cast<const sockaddr_in*>(addr)));
        }
    }

    void AttachConnectedSock(int sock) {
        close(sock_);
        sock_ = sock;
    }

    protected:
    static ssize_t ReadAll(int blocking_fd, char* buf, size_t size) {
        size_t size_read = 0;
        while(size_read < size) {
            ssize_t n = read(blocking_fd, buf + size_read, size - size_read);
            if(n <= 0) {
                return n;
            }
            size_read += n;
        }
        return size_read;
    }

    static bool WriteAll(int blocking_fd, const char* buf, size_t size) {
        size_t size_write = 0;
        while(size_write < size) {
            ssize_t n = write(blocking_fd, buf + size_write, size - size_write);
            if(n == -1) {
                std::cerr<<"write error: " << strerror(errno) << std::endl;
                return false;
            }
            size_write += n;
        }
        return true;
    }

    private:
    int sock_;
};

}
}

#endif  // WISSBI_IO_POLICY_TCP_HPP_
