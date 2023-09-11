#pragma once
#include <iostream>
#include <sstream>
#include "imp.h"

namespace lixiang_serialization {

struct direction_serialization_t {};

struct direction_deserialization_t {};

template <typename T>
struct get_serialization_type {
};

template<>
struct get_serialization_type<std::ostringstream> {
  typedef direction_serialization_t direction_t;
};

template<>
struct get_serialization_type<std::istringstream> {
  typedef direction_deserialization_t direction_t;
};


template <typename STREAM>
class serialization {
 public:
  STREAM &stream;
  serialization(STREAM &_stream) : stream(_stream) {
  }

  template<typename FIRST, typename ...OTHER>
  void do_serialization(FIRST &&first, OTHER&& ...other) {
    dump_to(stream, first);
    if constexpr (sizeof...(other) != 0) {
      do_serialization(std::forward<OTHER>(other)...);
    }
  }

  template<typename FIRST, typename ...OTHER>
  void do_deserialization(FIRST &&first, OTHER&& ...other) {
    parse_from(stream, first);
    if constexpr (sizeof...(other) != 0) {
      do_deserialization(std::forward<OTHER>(other)...);
    }
  }

  template <typename ...T>
  serialization<STREAM> &operator()(T&& ...args) {
    if constexpr (std::is_same_v<typename get_serialization_type<STREAM>::direction_t, direction_serialization_t>) {
      do_serialization(std::forward<T>(args)...);
    } else {
      do_deserialization(std::forward<T>(args)...);
    }
    return *this;
  }
};

}