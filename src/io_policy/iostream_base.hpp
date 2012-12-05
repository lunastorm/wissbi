#ifndef WISSBI_IO_POLICY_IOSTREAM_BASE_HPP_
#define WISSBI_IO_POLICY_IOSTREAM_BASE_HPP_

#include <iosfwd>
#include <thread>

namespace wissbi {
namespace io_policy {

class IOStreamBase
{
    public:
    IOStreamBase() : is_ptr_(&std::cin), os_ptr_(&std::cout) {
        std::thread([os_ptr_]{
            while(true) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                os_ptr_->flush();
            }
        }).detach();
    }

    void set_istream(std::istream *is_ptr) {
        is_ptr_ = is_ptr;
    }

    void set_ostream(std::ostream *os_ptr) {
        os_ptr_ = os_ptr;
    }

    protected:
    std::istream *is_ptr_;
    std::ostream *os_ptr_;
};

}
}

#endif  // WISSBI_IO_POLICY_IOSTREAM_BASE_HPP_
