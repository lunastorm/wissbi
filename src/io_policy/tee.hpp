#ifndef WISSBI_IO_POLICY_TEE_HPP_
#define WISSBI_IO_POLICY_TEE_HPP_

#include "msg_buf.hpp"
#include <unordered_map>
#include <memory>

namespace wissbi {
namespace io_policy {

template <class policy>
class Tee {
    public:
    Tee() {
    }

    ~Tee() {
    }

    bool Put(const MsgBuf &msg) {
        for(auto kv : branch_map_) {
            kv.second->Put(msg);
        }
        return true;
    }

    bool Get(MsgBuf *msg_ptr) {
    }

    void AddBranch(const std::string& branch_name, std::shared_ptr<policy> branch_ptr) {
        branch_map_[branch_name] = branch_ptr;
    }

    void RemoveBranch(const std::string& branch_name) {
        branch_map_.erase(branch_name);
    }

    bool ExistsBranch(const std::string& branch_name) {
        return branch_map_.find(branch_name) != branch_map_.end();
    }

    private:
    std::unordered_map<std::string, std::shared_ptr<policy>> branch_map_;
};

}
}

#endif  // WISSBI_IO_POLICY_TEE_HPP_
