project(lixiang)
cmake_minimum_required(VERSION 3.17)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -g -ggdb -std=c++17 -fno-strict-aliasing -Wno-register")

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -std=c++17 -fno-strict-aliasing -Wno-register")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -std=c++17 -fno-strict-aliasing -Wno-register")
find_package(Boost REQUIRED COMPONENTS json)
add_executable(test-serialization test.cpp)
target_link_libraries(test-serialization boost_json)
target_include_directories(test-serialization PUBLIC ${BOOST_INCLUDE_DIRS})