#include "model/BufferStream.hpp"

#include "model/Storable.hpp"

namespace UrSQL {

BufferWriter::BufferWriter(char* aBuf, size_type aSize)
    : m_buf(aBuf),
      m_pos(0),
      m_size(aSize) {}

BufferWriter& BufferWriter::operator<<(const std::string& aString) {
    size_type theLength = aString.length();
    (*this) << theLength;
    URSQL_TRUTH(m_pos + theLength < m_size, "BufferWriter out of range");
    memcpy(m_buf + m_pos, aString.data(), theLength);
    m_pos += theLength;
    return *this;
}

BufferWriter& BufferWriter::operator<<(const Storable& aStorable) {
    aStorable.serialize(*this);
    return *this;
}

BufferReader::BufferReader(const char* aBuf, size_type aSize)
    : m_buf(aBuf),
      m_pos(0),
      m_size(aSize) {}

BufferReader& BufferReader::operator>>(std::string& aString) {
    size_type theLength;
    (*this) >> theLength;
    URSQL_TRUTH(m_pos + theLength < m_size, "BufferReader out of range");
    aString.assign(m_buf + m_pos, theLength);
    m_pos += theLength;
    return *this;
}

BufferReader& BufferReader::operator>>(Storable& aStorable) {
    aStorable.deserialize(*this);
    return *this;
}

}  // namespace UrSQL