#include "BufferStream.hpp"
#include "Storable.hpp"

namespace UrSQL {

BufferWriter::BufferWriter(char* aBuf, size_type aSize) :
	m_buf(aBuf),
	m_pos(0),
	m_size(aSize)
{
}

BufferWriter& BufferWriter::operator<<(const std::string& aString) {
	size_type theLength = aString.length();
	(*this) << theLength;
	if (m_pos + theLength < m_size) {
		memcpy(m_buf + m_pos, aString.data(), theLength);
		m_pos += theLength;
		return *this;
	}
	throw std::out_of_range("BufferWriter out of range");
}

BufferWriter& BufferWriter::operator<<(const Storable& aStorable) {
	aStorable.serialize(*this);
	return *this;
}

BufferReader::BufferReader(const char* aBuf, size_type aSize) :
	m_buf(aBuf),
	m_pos(0),
	m_size(aSize)
{
}

BufferReader& BufferReader::operator>>(std::string& aString) {
	size_type theLength;
	(*this) >> theLength;
	if (m_pos + theLength < m_size) {
		aString.assign(m_buf + m_pos, theLength);
		m_pos += theLength;
		return *this;
	}
	throw std::out_of_range("BufferReader out of range");
}

BufferReader& BufferReader::operator>>(Storable& aStorable) {
	aStorable.deserialize(*this);
	return *this;
}

} /* UrSQL */