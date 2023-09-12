#pragma once
#include <iostream>
#include <type_traits>
#include "common.h"

namespace lixiang_serialization {

// How to serialization std::string

template<typename T>
struct operation_imp {
  static void dump_to(json_archive &s, nvp<T> &&value) {
    rapidjson::Value json_value;
    json_value.SetString("asdf");
    rapidjson::GenericValue a = value.second;
    s.AddMember(a, json_value, s.GetAllocator());
  }

  static T parse_from(json_archive &s, std::string name) {
    if (s.HasMember(name.c_str())) {
      if constexpr (std::is_same_v<T, int>) {
        return s.FindMember(name.c_str())->value.GetInt();
      } else if constexpr (std::is_same_v<T, std::string>) {
        return s.FindMember(name.c_str())->value.GetString();
      }
    }
    throw std::runtime_error("file not exists");
  }
};
}
