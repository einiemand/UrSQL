#include "BufferStream.hpp"

namespace UrSQL {

BufferWriter::BufferWriter(char* aBuf, size_type aSize) :
	m_Buf(aBuf),
	m_pos(0),
	m_size(aSize)
{
}

BufferWriter& BufferWriter::operator<<(const std::string& aString) {
	size_type theLength = aString.length();
	(*this) << theLength;
	if (m_pos + theLength < m_size) {
		memcpy(m_Buf + m_pos, aString.data(), theLength);
		m_pos += theLength;
		return *this;
	}
	throw std::out_of_range("BufferWriter out of range");
}

BufferReader::BufferReader(const char* aBuf, size_type aSize) :
	m_Buf(aBuf),
	m_pos(0),
	m_size(aSize)
{
}

BufferReader& BufferReader::operator>>(std::string& aString) {
	size_type theLength;
	(*this) >> theLength;
	if (m_pos + theLength < m_size) {
		aString.assign(m_Buf + m_pos, theLength);
		m_pos += theLength;
		return *this;
	}
	throw std::out_of_range("BufferReader out of range");
}

} /* UrSQL */