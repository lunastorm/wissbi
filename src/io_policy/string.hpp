#ifndef WISSBI_IO_POLICY_STRING_HPP_
#define WISSBI_IO_POLICY_STRING_HPP_

#include "io_policy/length.hpp"
#include <sstream>
#include <cmath>

namespace wissbi {
namespace io_policy {

class String : public Length
{
    public:
    String() {
        set_istream(new std::istream(&buf_));
        set_ostream(new std::ostream(&buf_));
    }

    void Send(const std::string& msg) {
        std::ostringstream oss;
        oss << msg.length() << " " << msg;
        buf_.str(oss.str());
    }

    std::string&& Receive() {
        std::istream is(&buf_);
        int size;
        (*is_ptr_) >> size;
        int header_len = int(log10(size)) + 2;
        std::string msg(buf_.str().c_str() + header_len, size);
        return std::move(msg);
    }

    private:
    std::stringbuf buf_;
};

}
}

#endif  // WISSBI_IO_POLICY_STRING_HPP_
