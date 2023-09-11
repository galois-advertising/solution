#pragma once
#include <iostream>
#include <type_traits>
#include "common.h"

namespace lixiang_serialization {

// How to serialization std::string
template<typename ARCHIVE>
void dump_to(ARCHIVE &s, const std::string &value) {
  for (const auto byte : make_head(object_type_t::string, value.size())) {
    append(s, byte);
  }
  for (auto ch : value) {
    append(s, ch);
  }
}

template<typename ARCHIVE>
void parse_from(ARCHIVE &s, std::string &value) {
  serialization_head_t head;
  for (auto i = 0; i < head.size(); ++i) {
    head[i] = next(s);
  }
  auto [type, size] = parse_head(head);
  assert(type == object_type_t::string);
  for (auto i = 0; i < size; i++) {
    value += next(s);
  }
}

// How to serialization fundamental value
template<typename ARCHIVE, typename T, typename std::enable_if<std::is_fundamental<T>::value, bool>::value>
void dump_to(ARCHIVE &s, const T &value) {
  for (const auto byte : make_head(object_type_t::fundamental, sizeof(T))) {
    append(s, byte);
  }
  auto pos = reinterpret_cast<const uint8_t *>(&value);
  for (auto i = 0; i < sizeof(T); i++) {
    append(s, *(pos + i));
  }
}

template<typename ARCHIVE, typename T, typename std::enable_if<std::is_fundamental<T>::value, bool>::value>
void parse_from(ARCHIVE &s, T &value) {
  serialization_head_t head;
  for (auto i = 0; i < head.size(); ++i) {
    head[i] = next(s);
  }
  auto [type, size] = parse_head(head);
  auto pos = reinterpret_cast<uint8_t *>(&value);
  for (auto i = 0; i < sizeof(T); i++) {
    *(pos + i) = next(s);
  }
}
}
