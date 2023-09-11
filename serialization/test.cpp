#include "serialization.h"
#include <vector>
using namespace lixiang_serialization;

struct Person;

struct Singer {
  std::string type;
  int age;

 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer(type, age);
  }
};

struct Address {
  std::string country;
  std::string city;
  std::string street;
  std::vector<Person> neighbors;
};

struct Friend {
  std::string relation;
  json::any secret;
};

struct Person {
  std::string name;
  int age;
  Address address;
  std::vector<Friend> _friends;
  json::any secret;
};

int main() {
  Friend f1{"my best friend", Singer{"rocker", 18}};
  Friend f2{"new friend", "little girl"};
  Friend f3{"third friend", 3};
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  Address addr1{"china", "beijing", "wangjing", {p2}};
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};

  int a;
  serialization<int> s(a);
  Singer singer;
  singer.serialization(s);

  // TODO. 以下是伪代码，需要笔试者具体实现 // auto json = dump(p1)
  // std::cout << json << std::endl // std::cout << p1 << std::endl
  // auto pp = parse(json);
  // assert(p1 == pp)
  // 序列化
  // 打印序列化结果
  // 打印 Person 对象 // 反序列化
  // 反序列化的结果是对的
}

