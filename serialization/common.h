#pragma once
#include <bitset>
#include <array>
#include "document.h"

namespace lixiang_serialization {
template <typename T>
using nvp = std::pair<T, std::string>;

#define _nvp(name) std::make_pair(name, #name)

/**
 * Json archive type
 */
typedef rapidjson::Document json_archive;
}