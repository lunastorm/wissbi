#ifndef WISSBI_LOGGER_HPP_
#define WISSBI_LOGGER_HPP_

#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <mutex>

namespace wissbi {
namespace logger {

static std::mutex mutex;

namespace priv {
template <typename T>
void dispatch_r(std::list<std::string>::iterator iter,
                std::list<std::string>::iterator end, std::ostringstream &oss,
                const T &last) {
  oss << *iter << last;
  oss << *(++iter);
  if (++iter != end) {
    throw std::invalid_argument("not enough argument given");
  }
}

template <typename U, typename... T>
void dispatch_r(std::list<std::string>::iterator iter,
                std::list<std::string>::iterator end, std::ostringstream &oss,
                const U &head, const T &... tail) {
  oss << *iter << head;
  dispatch_r(++iter, end, oss, tail...);
}
}

template <typename U, typename... T>
void log(const std::string &format, const U &head, const T &... tail) {
  std::list<std::string> segments;
  size_t seg_begin_pos = 0, seg_end_pos = 0;
  do {
    seg_end_pos = format.find("{}", seg_begin_pos);
    segments.push_back(
        format.substr(seg_begin_pos, seg_end_pos - seg_begin_pos));
    seg_begin_pos = seg_end_pos + 2;
  } while (seg_end_pos != std::string::npos);

  mutex.lock();
  using namespace std::chrono;
  std::time_t now = system_clock::to_time_t(system_clock::now());
  std::string time_str(std::ctime(&now));
  std::cerr << "[" << time_str.substr(0, time_str.length() - 1) << "] ";

  auto iter = segments.begin();
  std::ostringstream oss;
  priv::dispatch_r(iter, segments.end(), oss, head, tail...);
  std::cerr << oss.str() << std::endl;
  std::cerr.flush();

  mutex.unlock();
}

void log(const std::string &msg) {
  mutex.lock();
  using namespace std::chrono;
  std::time_t now = system_clock::to_time_t(system_clock::now());
  std::string time_str(std::ctime(&now));
  std::cerr << "[" << time_str.substr(0, time_str.length() - 1) << "] ";
  std::cerr << msg << std::endl;
  std::cerr.flush();

  mutex.unlock();
}

}
}

#endif // WISSBI_LOGGER_HPP_
