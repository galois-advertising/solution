#include "serialization.h"
#include <vector>
#include <any>
using namespace lixiang_serialization;

struct Person;

struct Singer {
  std::string type;
  int age;

 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(type), _nvp(age));
  }
};

struct Person {
  std::string name;
  int age;
public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(name), _nvp(age));
  }
};
struct Address {
  std::string country;
  std::string city;
  std::string street;
  std::vector<Person> neighbor;
  serializer::json_any secret;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(country), _nvp(city), _nvp(street));
    serializer.do_serialization(_nvp(neighbor), _nvp(secret));
  }
};

struct Friend {
  std::string relation;
  serializer::json_any secret;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(relation), _nvp(secret));
  }
};


int main() {
  Address addr1{"china", "beijing", "wangjing", {{"name1", 12}, {"name2", 13}}, Singer{"aa", 12}};

  json_archive data;
  serializer ser(data);
  // serialization to data
  ser.direction = direction_t::serialization;
  addr1.serialization(ser);
  std::cout << data << std::endl;

  // deserialization from data
  Address addr2;
  ser.direction = direction_t::deserialization;
  addr2.serialization(ser);
  std::cout << addr2.city;
  Singer singer2;
  addr2.secret.any_cast(singer2);
  std::cout << singer2.type;
}

