#pragma once
#include <iostream>
#include <sstream>
#include "common.h"
#include "imp.h"

namespace lixiang_serialization {

enum class direction_t : uint8_t {
  serialization, deserialization
};

template <typename ARCHIVE>
class serialization {
 public:
  ARCHIVE &archive;

  direction_t direction;

  serialization(ARCHIVE &_stream) : archive(_stream) {
  }

  template<typename FIRST, typename ...OTHER>
  void serialization_helper(FIRST &&first, OTHER&& ...other) {
    dump_to(archive, first);
    if constexpr (sizeof...(other) != 0) {
      serialization_helper(std::forward<OTHER>(other)...);
    }
  }

  template<typename FIRST, typename ...OTHER>
  void deserialization_helper(FIRST &&first, OTHER&& ...other) {
    parse_from(archive, first);
    if constexpr (sizeof...(other) != 0) {
      deserialization_helper(std::forward<OTHER>(other)...);
    }
  }

  template<typename ...TS>
  void do_serialization(TS &&...args) {
    if (direction == direction_t::serialization) {
      serialization_helper(std::forward<TS>(args)...);
    } else {
      deserialization_helper(std::forward<TS>(args)...);
    }
  }
};

}