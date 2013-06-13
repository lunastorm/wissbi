#ifndef WISSBI_SUB_ENTRY_HPP_
#define WISSBI_SUB_ENTRY_HPP_

#include "util.hpp"
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <algorithm>
#include <string>
#include <stdexcept>

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
        unlink(node_name_.c_str());
    }

    void renew() const {
        renew_();
    }

    private:
    void renew_() const {
        mkdir((meta_dir_ + "/sub/" + queue_name_).c_str(), S_IRWXU);
        int fd = open(node_name_.c_str(), O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        if(fd == -1) {
            throw std::runtime_error(std::string("Cannot create subscriber entry at ") + node_name_);
        }
        int res = futimes(fd, NULL);
        close(fd);
        if(res == -1) {
            throw std::runtime_error(std::string("Cannot update subscriber entry at ") + node_name_);
        }
    }

    std::string meta_dir_;
    std::string queue_name_;
    std::string node_name_;
    std::string addr_str_;
};

}

#endif  // WISSBI_SUB_ENTRY_HPP_
