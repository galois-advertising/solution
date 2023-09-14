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

class serializer {
  json_archive &archive;
 public:
  direction_t direction;

  class json_any : public boost::json::object {
    template<typename T>
    void reset(T &&b) {
      serializer ser(*this);
      ser.direction = direction_t::serialization;
      b.serialization(ser);
    }
  public:
    template<typename T>
    void any_cast(T &value) {
      serializer ser(*this);
      ser.direction = direction_t::deserialization;
      value.serialization(ser);
    }

    template<typename T>
    json_any(const T &b) {
      reset(b);
    }

    json_any() : boost::json::object() {}

    template<typename T>
    json_any(T &&b) {
      reset(std::forward<T>(b));
    }

    template<typename T>
    json_any &operator=(T &&data) {
      reset(std::forward<T>(data));
      return *this;
    }

    template<typename T>
    json_any &operator=(const T &data) {
      reset(data);
      return *this;
    }
  };

  serializer(json_archive &_stream) : archive(_stream) {
  }

  template<typename T>
  static void dump_to(json_archive &s, nvp<T> &&value) {
    using TT = typename std::remove_reference<T>::type;
    if constexpr (std::is_fundamental_v<TT> || std::is_same_v<TT, std::string> || std::is_same_v<TT, json_any>) {
      s[value.second] = value.first;
    } else if constexpr(is_std_vector<TT>::value) {
      boost::json::array array;
      for (auto &item : value.first) {
        json_archive temp;
        serializer ser(temp);
        ser.direction = direction_t::serialization;
        item.serialization(ser);
        array.push_back(temp);
      }
      s[value.second] = array;
    } else {
      json_archive temp;
      serializer ser(temp);
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
          serializer ser(item.as_object());
          ser.direction = direction_t::deserialization;
          temp.serialization(ser);
          value.first.push_back(std::move(temp));
        }
      } else if constexpr (std::is_same_v<TT, json_any>) {
        *static_cast<boost::json::object*>(&value.first) = s.at(value.second).as_object();
      } else {
        serializer ser(s.at(value.second).as_object());
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
