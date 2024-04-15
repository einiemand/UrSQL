#pragma once

#include <cstring>
#include <type_traits>

#include "common/Error.hpp"

namespace UrSQL {

class Storable;

namespace detail {

class BufferData {
public:
    BufferData(const char* aBuf, size_type aSize);
    BufferData(char* aBuf, size_type aSize);

    ~BufferData() = default;

    URSQL_DISABLE_COPY(BufferData);

    char* getAndAdvance(size_type anOffset);

private:
    union {
        const char* m_get;
        char* m_put;
    } m_cur;

    const char* const m_end;
};

}  // namespace detail

class BufferReader {
public:
    BufferReader(const char* aBuf, size_type aSize);
    ~BufferReader() = default;

    URSQL_DISABLE_COPY(BufferReader);

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> ||
                                                     std::is_enum_v<T>>>
    BufferReader& operator>>(T& aNum) {
        constexpr size_type theTypeSize = sizeof(T);
        memcpy(&aNum, m_bufData.getAndAdvance(theTypeSize), theTypeSize);
        return *this;
    }

    BufferReader& operator>>(std::string& aString);
    BufferReader& operator>>(Storable& aStorable);

    template<typename T>
    T read() {
        T theVal;
        *this >> theVal;
        return theVal;
    }

private:
    detail::BufferData m_bufData;
};

class BufferWriter {
public:
    BufferWriter(char* aBuf, size_type aSize);
    ~BufferWriter() = default;

    URSQL_DISABLE_COPY(BufferWriter);

    template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> ||
                                                     std::is_enum_v<T>>>
    BufferWriter& operator<<(const T aNum) {
        constexpr size_type theTypeSize = sizeof(T);
        memcpy(m_bufData.getAndAdvance(theTypeSize), &aNum, theTypeSize);
        return *this;
    }

    BufferWriter& operator<<(const std::string& aString);
    BufferWriter& operator<<(const Storable& aStorable);

private:
    detail::BufferData m_bufData;
};

}  // namespace UrSQL
