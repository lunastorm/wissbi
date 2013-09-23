#ifndef WISSBI_MSG_FILTER_HPP_
#define WISSBI_MSG_FILTER_HPP_

#include "msg_buf.hpp"
#include <functional>

namespace wissbi {

template <class input_policy> class InputWrapper : public input_policy {
public:
  using input_policy::Get;
};

template <class output_policy> class OutputWrapper : public output_policy {
public:
  using output_policy::Put;
};

template <class input_policy, class output_policy>
class MsgFilter : public InputWrapper<input_policy>,
                  public OutputWrapper<output_policy> {
  typedef std::function<bool()> PreFilterFuncType;
  typedef std::function<bool(MsgBuf &msg)> FilterFuncType;
  typedef std::function<bool(bool filter_result, MsgBuf &msg)>
      PostFilterFuncType;

public:
  MsgFilter()
      : pre_filter_func_([]() {
    return true;
  }),
        filter_func_([](MsgBuf & msg) {
    return true;
  }),
        post_filter_func_([](bool filter_result, MsgBuf & msg) {
    return filter_result;
  }) {}

  virtual bool Filter() {
    bool res =
        pre_filter_func_() && static_cast<InputWrapper<input_policy> *>(this)
                                  ->Get(&msg_buf_) && filter_func_(msg_buf_) &&
        static_cast<OutputWrapper<output_policy> *>(this)->Put(msg_buf_);
    return post_filter_func_(res, msg_buf_);
  }

  void FilterLoop() {
    while (Filter()) {
    }
  }

  void set_pre_filter_func(PreFilterFuncType pre_filter_func) {
    pre_filter_func_ = pre_filter_func;
  }

  void set_filter_func(FilterFuncType filter_func) {
    filter_func_ = filter_func;
  }

  void set_post_filter_func(PostFilterFuncType post_filter_func) {
    post_filter_func_ = post_filter_func;
  }

private:
  MsgBuf msg_buf_;
  PreFilterFuncType pre_filter_func_;
  FilterFuncType filter_func_;
  PostFilterFuncType post_filter_func_;
};

}

#endif // WISSBI_MSG_FILTER_HPP_
