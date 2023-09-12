#pragma once
#include <iostream>
#include <sstream>
#include "common.h"
#include "imp.h"

namespace lixiang_serialization {

enum class direction_t : uint8_t {
  serialization, deserialization
};

class serialization {
 public:
  json_archive &archive;

  direction_t direction;

  serialization(json_archive &_stream) : archive(_stream) {
  }

  template<typename FIRST, typename ...OTHER>
  void serialization_helper(FIRST &&arg1, OTHER&& ...args) {
    operation_imp<typename std::remove_reference_t<FIRST>::first_type>::dump_to(archive, arg1);
    if constexpr (sizeof...(args) != 0) {
      serialization_helper(std::forward<OTHER>(args)...);
    }
  }

  template<typename FIRST, typename ...OTHER>
  void deserialization_helper(FIRST &&arg1, OTHER&& ...args) {
    arg1.first = operation_imp<typename std::remove_reference_t<FIRST>::first_type>::parse_from(archive, arg1.second);
    if constexpr (sizeof...(args) != 0) {
      deserialization_helper(std::forward<OTHER>(args)...);
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