#pragma once
#include <iostream>

namespace lixiang_serialization {

template <typename STREAM>
class serialization {
  STREAM &stream;
  serialization(STREAM &_stream) : stream(_stream) {
  }

  template<typename FIRST, typename ...OTHER>
  void do_serialization(FIRST &&first, OTHER&& ...other) {
    if constexpr (sizeof...(other) == 0) {
      return;
    }
    std::cout << first << std::endl;
    do_serialization(other...);
  }

  template <typename ...T>
  serialization<STREAM> &operator()(T&& ...args) {
    do_serialization(std::forward<T>(args)...);
    return *this;
  }
};

}