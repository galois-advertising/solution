#pragma once
#include <iostream>
#include <type_traits>
namespace lixiang_serialization {
template<typename STREAM, typename T, typename = std::enable_if<std::is_fundamental<T>::value, void>>
static void dump_to(STREAM &s, const T &value) {
  auto pos = reinterpret_cast<const uint8_t *>(&value);
  for (auto i = 0; i < sizeof(T); i++) {
    s << *(pos + i);
  }
}

template<typename STREAM, typename T, typename = std::enable_if<std::is_fundamental<T>::value, void>>
static void parse_from(STREAM &s, const T &value) {
  auto pos = reinterpret_cast<const uint8_t *>(&value);
  for (auto i = 0; i < sizeof(T); i++) {
    s >> *(pos + i);
  }
}
}
