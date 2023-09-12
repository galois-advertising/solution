#include "serialization.h"
#include <vector>
#include <any>
#include "rapidjson.h"
#include "common.h"
using namespace lixiang_serialization;
using namespace rapidjson;

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

struct Address {
  std::string country;
  std::string city;
  std::string street;
  std::vector<Person> neighbors;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(country), _nvp(city), _nvp(street));
    serializer.do_serialization(neighbors);
  }
};

struct Friend {
  std::string relation;
  std::any secret;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(relation), _nvp(secret));
  }
};

struct Person {
  std::string name;
  int age;
  Address address;
  std::vector<Friend> _friends;
  std::any secret;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer.do_serialization(_nvp(name), _nvp(age), _nvp(address), _nvp(secret));
  }
};

int main() {
  Singer s{"it is a type str", 12};
  /*
  Friend f1{"my best friend", Singer{"rocker", 18}};
  Friend f2{"new friend", "little girl"};
  Friend f3{"third friend", 3};
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  Address addr1{"china", "beijing", "wangjing", {p2}};
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};
   */


  json_archive data;
  data.Parse("{}");
  serialization ser(data);
  ser.direction = direction_t::serialization;
  s.serialization(ser);
  Singer b;
  ser.direction = direction_t::deserialization;
  b.serialization(ser);
  std::cout << b.type;

  // TODO. 以下是伪代码，需要笔试者具体实现 // auto json = dump(p1)
  // std::cout << json << std::endl // std::cout << p1 << std::endl
  // auto pp = parse(json);
  // assert(p1 == pp)
  // 序列化
  // 打印序列化结果
  // 打印 Person 对象 // 反序列化
  // 反序列化的结果是对的
}

