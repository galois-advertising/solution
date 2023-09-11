#pragma once
#include <bitset>
#include <array>

namespace lixiang_serialization {
/**
 * Every item that was serialized must have a `head`
 * The higher bits(4 bits) stores item type part, and lower bits stores the size of item
 * In case of fundamental type: size is sizeof(T)
 * In case of std::string type: size is length of string
 * In case of list type: size is length of list
 *
 * head_size: size of head. eg:16
 * type_size: size of type part. eg 4
 */
typedef unsigned short head_type_t;
typedef std::array<uint8_t, sizeof(head_type_t)> serialization_head_t;
const size_t head_size = sizeof(head_type_t) * 8;
const size_t type_size = 4;
const size_t type_mask = (1 << (head_size - type_size)) - 1;

enum class object_type_t : uint8_t {
  fundamental = 1, string = 2, list = 3
};

/**
 * make head by type and size
 */
inline serialization_head_t make_head(const object_type_t type, size_t size) {
  head_type_t head = 0;
  head |= static_cast<head_type_t>(type) << (head_size - type_size);
  head |= size;
  serialization_head_t head_array;
  for (auto pos = 0; pos < head_array.size(); ++pos) {
    head_array[pos] = *(reinterpret_cast<uint8_t*>(&head) + pos);
  }
  return head_array;
}

/**
 * parse head from array
 */
inline std::pair<object_type_t, size_t> parse_head(const serialization_head_t &head_array) {
  head_type_t head = 0;
  for (auto pos = 0; pos < head_array.size(); ++pos) {
     *(reinterpret_cast<uint8_t*>(&head) + pos) = head_array[pos];
  }
  auto type = static_cast<object_type_t>(head >> (head_size - type_size));
  head &= type_mask;
  return { type, head };
}

/**
 * Default archive type
 * First: content
 * Second: current read position
 */
typedef std::pair<std::string, size_t> default_archive;

/**
 * Append data to archive
 */
template <typename T>
void append(T &archive, uint8_t data) {
}

/**
 * Get next data from archive
 */
template <typename T>
uint8_t next(T &archive) {
  return 0;
}

/**
 * append & next imp for default_archive
 */
template<>
void append<default_archive>(default_archive &archive, uint8_t data) {
  archive.first += data;
}

template <>
uint8_t next<default_archive>(default_archive &archive) {
  return archive.first[archive.second++];
}
}