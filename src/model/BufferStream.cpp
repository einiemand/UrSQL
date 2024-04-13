#include "model/BufferStream.hpp"

#include "model/Storable.hpp"

namespace UrSQL {

namespace detail {

BufferData::BufferData(const char* aBuf, size_type aSize)
    : m_cur{ aBuf },
      m_end(aBuf + aSize) {}

BufferData::BufferData(char* aBuf, size_type aSize)
    : BufferData(static_cast<const char*>(aBuf), aSize) {}

char* BufferData::getAndAdvance(size_type anOffset) {
    char* theCur = m_cur.m_put;
    m_cur.m_put += anOffset;
    URSQL_TRUTH(m_cur.m_put <= m_end, "BufferData cursor out of range");
    return theCur;
}

}  // namespace detail

BufferReader::BufferReader(const char* aBuf, size_type aSize)
    : m_bufData(aBuf, aSize) {}

BufferReader& BufferReader::operator>>(std::string& aString) {
    size_type theLength;
    (*this) >> theLength;
    aString.assign(m_bufData.getAndAdvance(theLength), theLength);
    return *this;
}

BufferReader& BufferReader::operator>>(Storable& aStorable) {
    aStorable.deserialize(*this);
    return *this;
}

BufferWriter::BufferWriter(char* aBuf, size_type aSize)
    : m_bufData(aBuf, aSize) {}

BufferWriter& BufferWriter::operator<<(const std::string& aString) {
    size_type theLength = aString.length();
    (*this) << theLength;
    memcpy(m_bufData.getAndAdvance(theLength), aString.data(), theLength);
    return *this;
}

BufferWriter& BufferWriter::operator<<(const Storable& aStorable) {
    aStorable.serialize(*this);
    return *this;
}

}  // namespace UrSQL
