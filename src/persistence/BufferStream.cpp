#include "persistence/BufferStream.hpp"

#include <iostream>

#include "common/Messaging.hpp"
#include "exception/InternalError.hpp"
#include "model/Storable.hpp"

namespace ursql {

namespace detail {

BufferData::BufferData(const char* cbuf, std::size_t size)
    : cbuf_(cbuf),
      end_(cbuf + size) {}

BufferData::BufferData(char* buf, std::size_t size)
    : BufferData(static_cast<const char*>(buf), size) {}

char* BufferData::getAndAdvance(std::size_t offset) {
    char* p = buf_;
    buf_ += offset;
    URSQL_ASSERT(buf_ <= end_, "BufferData cursor out of range");
    return p;
}

}  // namespace detail

BufferReader::BufferReader(const char* cbuf, std::size_t size)
    : bufData_(cbuf, size) {}

BufferReader& BufferReader::operator>>(std::string& str) {
    auto len = read<std::size_t>();
    str.assign(bufData_.getAndAdvance(len), len);
    return *this;
}

BufferReader& BufferReader::operator>>(ursql::Storable& storable) {
    storable.deserialize(*this);
    return *this;
}

BufferWriter::BufferWriter(char* buf, std::size_t size) : bufData_(buf, size) {}

BufferWriter& BufferWriter::operator<<(const std::string& str) {
    std::size_t len = str.length();
    (*this) << len;
    memcpy(bufData_.getAndAdvance(len), str.data(), len);
    return *this;
}

BufferWriter& BufferWriter::operator<<(const Storable& storable) {
    storable.serialize(*this);
    return *this;
}

}  // namespace ursql
