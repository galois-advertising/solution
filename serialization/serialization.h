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

  struct json_any : public std::any {
    std::function<json_archive()> dumper;
    json_archive archive;

    template <typename T>
    json_archive do_dump() {
      json_archive temp;
      auto &any_value = std::any_cast<T&>(*this);
      dump_to(temp, _nvp(any_value));
      return temp;
    }
  public:
    json_any() : std::any() {
      dumper = [&](){ return json_archive(); };
    }

    template <typename T>
    json_any(const T &b) : std::any(b) {
      dumper = [&](){ return do_dump<T>(); };
    }

    template<typename T>
    json_any(T &&b) : std::any(std::forward<T>(b)) {
      dumper = [&](){ return do_dump<T>(); };
    }

    template<typename T>
    json_any &operator=(T &&data) {
      std::any::operator=(std::forward<T>(data));
      dumper = [&](){ return do_dump<T>(); };
      return *this;
    }

    template<typename T>
    json_any &operator=(const T &data) {
      std::any::operator=(data);
      dumper = [&](){ do_dump<T>(); };
      return *this;
    }
  };

  serializer(json_archive &_stream) : archive(_stream) {
  }

  template<typename T>
  static void dump_to(json_archive &s, nvp<T> &&value) {
    using TT = typename std::remove_reference<T>::type;
    if constexpr (std::is_fundamental_v<TT> || std::is_same_v<TT, std::string>) {
      s[value.second] = value.first;
    } else if constexpr(std::is_same_v<TT, json_any>) {
      s[value.second] = value.first.dumper();
    } else if constexpr(is_std_vector<TT>::value) {
      boost::json::array array;
      for (auto &item : value.first) {
        json_archive temp;
        dump_to(temp, _nvp(item));
        array.push_back(temp.at("item"));
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
        value.first = static_cast<TT>(s.at(value.second).as_int64());
      } else if constexpr (std::is_same_v<TT, double> || std::is_same_v<TT, double>) {
        value.first = static_cast<TT>(s.at(value.second).as_double());
      } else if constexpr (std::is_same_v<TT, std::string>) {
        value.first = s.at(value.second).as_string().c_str();
      } else if constexpr(is_std_vector<TT>::value) {
        value.first.clear();
        for (auto &item : s.at(value.second).as_array()) {
          typename TT::value_type temp;
          json_archive temp_archive{{"temp", item}};
          parse_from(temp_archive, _nvp(temp));
          value.first.push_back(temp);
        }
      } else if constexpr (std::is_same_v<TT, json_any>) {
        value.first.archive = s.at(value.second).as_object();
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
