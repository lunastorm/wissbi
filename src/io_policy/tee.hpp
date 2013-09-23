#ifndef WISSBI_IO_POLICY_TEE_HPP_
#define WISSBI_IO_POLICY_TEE_HPP_

#include "msg_buf.hpp"
#include <unordered_map>
#include <memory>

namespace wissbi {
namespace io_policy {

template <class policy> class Tee {
public:
  Tee() {}

  ~Tee() {}

  bool Put(const MsgBuf &msg) {
    last_tee_count_ = 0;
    last_failed_count_ = 0;
    for (auto kv : branch_map_) {
      ++last_tee_count_;
      if (!kv.second->Put(msg)) {
        ++last_failed_count_;
      }
    }
    return true;
  }

  bool Get(MsgBuf *msg_ptr) {}

  void AddBranch(const std::string &branch_name,
                 std::shared_ptr<policy> branch_ptr) {
    branch_map_[branch_name] = branch_ptr;
  }

  void RemoveBranch(const std::string &branch_name) {
    branch_map_.erase(branch_name);
  }

  bool ExistsBranch(const std::string &branch_name) {
    return branch_map_.find(branch_name) != branch_map_.end();
  }

  int GetBranchCount() { return branch_map_.size(); }

  int GetLastTeeCount() { return last_tee_count_; }

  int GetLastFailedCount() { return last_failed_count_; }

private:
  std::unordered_map<std::string, std::shared_ptr<policy> > branch_map_;
  int last_tee_count_;
  int last_failed_count_;
};

}
}

#endif // WISSBI_IO_POLICY_TEE_HPP_
