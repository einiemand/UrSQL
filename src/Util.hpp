#pragma once
#ifndef UTIL_HPP
#define UTIL_HPP
#include <xutility>
#include <stdint.h>
#include <vector>
#include <string>

namespace UrSQL {

using size_type = std::size_t;

#if _WIN64
using blocknum_t = int32_t;
#elif _WIN32
using blocknum_t = int64_t;
#endif

using StringList = std::vector<std::string>;

}

#endif