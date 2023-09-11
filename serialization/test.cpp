#include "serialization.h"
#include <vector>
#include "rapidjson.h"
#include "reader.h"
#include <sstream>
using namespace lixiang_serialization;
using namespace rapidjson;


struct json {
  struct any {
    std::string value;
    any() : value("empty") {
    }

    template<typename T>
    any(const T &v) {
      std::stringstream ss;
      ss << "[Some object of size " << sizeof(T) << "]";
      value = ss.str();
    }

    template<typename T>
    any(const T &&v) {
      std::stringstream ss;
      ss << "[Some object of size " << sizeof(T) << "]";
      value = ss.str();
    }
  };
};
namespace lixiang_serialization {
template<typename STREAM>
static void dump_to(STREAM &s, const json::any &value) {
  s << value;
}

template<typename STREAM>
static void parse_from(STREAM &s, const json::any &value) {
  s >> value;
}
}

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
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer(country, city, street);
    serializer(neighbors);
  }
};

struct Friend {
  std::string relation;
  json::any secret;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer(relation, secret);
  }
};

struct Person {
  std::string name;
  int age;
  Address address;
  std::vector<Friend> _friends;
  json::any secret;
 public:
  template<typename S>
  void serialization(S &serializer) {
    serializer(name, age, address, secret);
  }
};

int main() {
  Friend f1{"my best friend", Singer{"rocker", 18}};
  Friend f2{"new friend", "little girl"};
  Friend f3{"third friend", 3};
  Person p2{"p2", 3, Address{"china", "shanghai", "putuo"}};
  Address addr1{"china", "beijing", "wangjing", {p2}};
  Person p1{"p1", 4, addr1, {f1, f2, f3}, "the kind!"};


  std::ostringstream output;
  serialization s(output);
  p2.serialization(s);
  Person b;
  std::istringstream input(output.str());
  b.serialization(input);
  std::cout << b.name;

  // TODO. 以下是伪代码，需要笔试者具体实现 // auto json = dump(p1)
  // std::cout << json << std::endl // std::cout << p1 << std::endl
  // auto pp = parse(json);
  // assert(p1 == pp)
  // 序列化
  // 打印序列化结果
  // 打印 Person 对象 // 反序列化
  // 反序列化的结果是对的
}

