#ifndef WISSBI_SUB_DIR_HPP_
#define WISSBI_SUB_DIR_HPP_

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <vector>

namespace wissbi {

class SubDir {
    public:
    SubDir(const std::string& meta_dir, const std::string& queue_name) :
        meta_dir_(meta_dir), queue_name_(queue_name)
    {
    }

    ~SubDir() {
    }

    std::vector<std::string> GetSubList() {
        std::vector<std::string> res;
        DIR *dir_ptr = opendir((meta_dir_ + "/sub/" + queue_name_).c_str());
        if(dir_ptr != NULL) {
            struct dirent *ent = NULL;
            while((ent = readdir(dir_ptr)) != NULL) {
                if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0 || ent->d_type == DT_DIR) {
                    continue;
                }
                res.push_back(ent->d_name);
            }
            closedir(dir_ptr);
        }
        return res;
    }

    private:
    std::string meta_dir_;
    std::string queue_name_;
};

}

#endif  // WISSBI_SUB_DIR_HPP_
