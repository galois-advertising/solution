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
  int id;
  double location;
  std::string country;
  std::string city;
  std::string street;
  std::vector<Person> neighbor;
  serializer::json_any secret1;
  serializer::json_any secret2;
  serializer::json_any secret3;
  serializer::json_any secret4;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(id), _nvp(location), _nvp(country), _nvp(city), _nvp(street), _nvp(neighbor));
    serializer.do_serialization(_nvp(secret1), _nvp(secret2), _nvp(secret3), _nvp(secret4));
  }
};

int main() {
  std::vector<int> int_vec{1, 2, 3, 4};
  std::vector<Singer> class_vec{{"nnn", 13}};
  Address addr1{1, 1.2, "china", "beijing", "wangjing",
                {{"name1", 12}, {"name2", 13}}, Singer{"aa", 12}, int_vec, class_vec, 3.1415};

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
  auto class_secret = addr2.secret1.any_cast<Singer>();
  auto int_vector_secret = addr2.secret2.any_cast<std::vector<int>>();
  auto class_vector_secret = addr2.secret3.any_cast<std::vector<Singer>>();
  auto double_secret = addr2.secret4.any_cast<double>();
  return 0;
}

