#ifndef WISSBI_SUB_ENTRY_HPP_
#define WISSBI_SUB_ENTRY_HPP_

#include "util.hpp"
#include <stdlib.h>
#include <algorithm>
#include <string>

namespace wissbi {

class SubEntry {
    public:
    SubEntry(const std::string& meta_dir, const std::string& queue_name, const std::string& addr_str) :
        meta_dir_(meta_dir), queue_name_(queue_name), addr_str_(addr_str)
    {
        node_name_ = meta_dir_ + "/sub/" + queue_name_ + "/" + addr_str_ + "," +
                     wissbi::util::EscapeSubFolderPath(queue_name_);
        renew_();
    }

    ~SubEntry() {
        system(("rm " + node_name_).c_str());
    }

    void renew() const {
        renew_();
    }

    private:

    void renew_() const {
        system(("mkdir " + meta_dir_ + "/sub/" + queue_name_ + " 2>/dev/null").c_str());
        if(system(("touch " + node_name_).c_str()) != 0) {
            throw std::string("Cannot create subscriber entry at ") + node_name_;
        }
    }

    std::string meta_dir_;
    std::string queue_name_;
    std::string node_name_;
    std::string addr_str_;
};

}

#endif  // WISSBI_SUB_ENTRY_HPP_
