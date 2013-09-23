#ifndef WISSBI_SUB_DIR_HPP_
#define WISSBI_SUB_DIR_HPP_

#include "util.hpp"
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>

namespace wissbi {

class SubDir {
public:
  SubDir(const std::string &meta_dir, const std::string &queue_name)
      : meta_dir_(meta_dir), queue_name_(queue_name) {}

  ~SubDir() {}

  std::vector<std::tuple<std::string, std::string> > GetSubList() {
    std::vector<std::tuple<std::string, std::string> > res;
    DIR *dir_ptr = opendir((meta_dir_ + "/sub/" + queue_name_).c_str());
    if (dir_ptr != NULL) {
      struct dirent *ent = NULL;
      while ((ent = readdir(dir_ptr)) != NULL) {
        if (strcmp(".", ent->d_name) == 0 || strcmp("..", ent->d_name) == 0 ||
            ent->d_type == DT_DIR) {
          continue;
        }
        std::string entry(ent->d_name);
        size_t sep_pos = entry.find(",");
        res.push_back(
            std::make_tuple(entry.substr(0, sep_pos),
                            wissbi::util::UnescapeSubFolderPath(entry.substr(
                                sep_pos + 1, entry.length() - sep_pos - 1))));
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

#endif // WISSBI_SUB_DIR_HPP_
