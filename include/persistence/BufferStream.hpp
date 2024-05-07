#pragma once

#include <cstring>
#include <type_traits>

#include "common/Macros.hpp"

namespace ursql {

class Storable;

namespace detail {

class BufferData {
public:
    BufferData(const char* cbuf, std::size_t size);
    BufferData(char* buf, std::size_t size);

    ~BufferData() = default;

    URSQL_DISABLE_COPY(BufferData);

    char* getAndAdvance(std::size_t offset);

private:
    union {
        const char* cbuf_;
        char* buf_;
    };

    const char* const end_;
};

}  // namespace detail

class BufferReader {
public:
    BufferReader(const char* buf, std::size_t size);
    ~BufferReader() = default;

    URSQL_DISABLE_COPY(BufferReader);

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> ||
                                                     std::is_enum_v<T>>>
    BufferReader& operator>>(T& val) {
        constexpr const std::size_t typeSize = sizeof(T);
        memcpy(&val, bufData_.getAndAdvance(typeSize), typeSize);
        return *this;
    }

    BufferReader& operator>>(std::string& str);
    BufferReader& operator>>(Storable& storable);

    template<typename T>
    T read() {
        T val;
        *this >> val;
        return val;
    }

private:
    detail::BufferData bufData_;
};

class BufferWriter {
public:
    BufferWriter(char* buf, std::size_t size);
    ~BufferWriter() = default;

    URSQL_DISABLE_COPY(BufferWriter);

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> ||
                                                     std::is_enum_v<T>>>
    BufferWriter& operator<<(const T val) {
        constexpr const std::size_t typeSize = sizeof(T);
        memcpy(bufData_.getAndAdvance(typeSize), &val, typeSize);
        return *this;
    }

    BufferWriter& operator<<(const std::string& str);
    BufferWriter& operator<<(const Storable& storable);

private:
    detail::BufferData bufData_;
};

}  // namespace ursql
