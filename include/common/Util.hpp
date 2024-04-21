#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "Macros.hpp"

namespace ursql {

using size_type = std::size_t;

template<size_type word_size>
struct size_trait {};

template<>
struct size_trait<4> {
    using int_t = int32_t;
};

template<>
struct size_trait<8> {
    using int_t = int64_t;
};

using blocknum_t = size_trait<sizeof(void*)>::int_t;

}  // namespace ursql
