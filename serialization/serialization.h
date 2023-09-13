#pragma once
#include <any>
#include <iostream>
#include <boost/json.hpp>
#include <sstream>
#include <typeindex>

namespace lixiang_serialization {
template <typename T>
using nvp = std::pair<T&, std::string>;

#define _nvp(name) std::make_pair(std::ref(name), std::string(#name))

enum class direction_t : uint8_t {
  serialization, deserialization
};

typedef boost::json::object json_archive;

template<typename>
struct is_std_vector : std::false_type {};

template<typename T, typename A>
struct is_std_vector<std::vector<T,A>> : std::true_type {};


class serialization {
 public:
  json_archive &archive;
  direction_t direction;

  serialization(json_archive &_stream) : archive(_stream) {
  }

  template<typename T>
  static void dump_to(json_archive &s, nvp<T> &&value) {
    using TT = typename std::remove_reference<T>::type;
    if constexpr (std::is_fundamental_v<TT> || std::is_same_v<TT, std::string>) {
      s[value.second] = value.first;
    } else if constexpr(is_std_vector<TT>::value) {
      boost::json::array array;
      for (auto &item : value.first) {
        json_archive temp;
        serialization ser(temp);
        ser.direction = direction_t::serialization;
        item.serialization(ser);
        array.push_back(temp);
      }
      s[value.second] = array;
    } else if constexpr (std::is_same_v<TT, std::any>) {
      std::any &var = value.first;
      if (!var.has_value()) {
        return;
      }
      boost::json::object any_value;
      any_value["typeid"] = std::type_index(var.type()).name();
      any_value["content"] = "[context of any object]";
      s[value.second] = any_value;
    } else {
      json_archive temp;
      serialization ser(temp);
      ser.direction = direction_t::serialization;
      value.first.serialization(ser);
      s[value.second] = temp;
    }
  }

  template<typename T>
  static void parse_from(json_archive &s, nvp<T> &&value) {
    using TT = typename std::remove_reference<T>::type;
    if (s.contains(value.second.c_str())) {
      if constexpr (std::is_same_v<TT, int>) {
        value.first = static_cast<int>(s.at(value.second).as_int64());
      } else if constexpr (std::is_same_v<TT, std::string>) {
        value.first = s.at(value.second).as_string().c_str();
      } else if constexpr(is_std_vector<TT>::value) {
        value.first.clear();
        for (auto &item : s.at(value.second).as_array()) {
          typename TT::value_type temp;
          serialization ser(item.as_object());
          ser.direction = direction_t::deserialization;
          temp.serialization(ser);
          value.first.push_back(std::move(temp));
        }
      } else if constexpr (std::is_same_v<TT, std::any>) {
        auto item = s.at(value.second).as_object();
      } else {
        serialization ser(s.at(value.second).as_object());
        ser.direction = direction_t::deserialization;
        value.first.serialization(ser);
      }
    } else {
      throw std::runtime_error("field not exists");
    }
  }

  template<typename FIRST, typename ...OTHER>
  void serialization_helper(FIRST &&arg1, OTHER&& ...args) {
    dump_to<typename std::remove_reference_t<FIRST>::first_type>(archive, std::forward<FIRST>(arg1));
    if constexpr (sizeof...(args) != 0) {
      serialization_helper(std::forward<OTHER>(args)...);
    }
  }

  template<typename FIRST, typename ...OTHER>
  void deserialization_helper(FIRST &&arg1, OTHER&& ...args) {
    parse_from<typename std::remove_reference_t<FIRST>::first_type>(archive, std::forward<FIRST>(arg1));
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
