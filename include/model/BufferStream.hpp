#pragma once

#include "common/Error.hpp"
#include <type_traits>
#include <cstring>

namespace UrSQL {

class Storable;

class BufferWriter {
public:
	BufferWriter(char* aBuf, size_type aSize);
	~BufferWriter() = default;

	URSQL_DISABLE_COPY(BufferWriter);

	template<typename T, typename = std::enable_if_t< std::is_arithmetic_v<T> > >
	BufferWriter& operator<<(const T aNum) {
		static constexpr size_type theTypeSize = sizeof(T);
		URSQL_TRUTH(m_pos + theTypeSize < m_size, "BufferWriter out of range");
		memcpy(m_buf + m_pos, &aNum, theTypeSize);
		m_pos += theTypeSize;
		return *this;
	}

	BufferWriter& operator<<(const std::string& aString);
	BufferWriter& operator<<(const Storable& aStorable);

private:
	char* m_buf;
	size_type m_pos;
	size_type m_size;
};

class BufferReader {
public:
	BufferReader(const char* aBuf, size_type aSize);
	~BufferReader() = default;

	URSQL_DISABLE_COPY(BufferReader);

	template<typename T, typename = std::enable_if_t< std::is_arithmetic_v<T> >>
	BufferReader& operator>>(T& aNum) {
		static constexpr size_type theTypeSize = sizeof(T);
		URSQL_TRUTH(m_pos + theTypeSize < m_size, "BufferReader out of range");
		memcpy(&aNum, m_buf + m_pos, theTypeSize);
		m_pos += theTypeSize;
		return *this;
	}

	BufferReader& operator>>(std::string& aString);
	BufferReader& operator>>(Storable& aStorable);

private:
	const char* m_buf;
	size_type m_pos;
	size_type m_size;
};

}
