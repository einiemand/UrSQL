#pragma once
#ifndef BUFFERSTREAM_HPP
#define BUFFERSTREAM_HPP
#include "../Error.hpp"

namespace UrSQL {

class Storable;

class BufferWriter {
public:
	BufferWriter(char* aBuf, size_type aSize);
	~BufferWriter() = default;

	BufferWriter(const BufferWriter&) = delete;
	BufferWriter& operator=(const BufferWriter&) = delete;

	template<typename T, typename = std::enable_if_t< std::is_arithmetic_v<T> >>
	BufferWriter& operator<<(const T aNum) {
		static constexpr size_type theTypeSize = sizeof(T);
		if (m_pos + theTypeSize < m_size) {
			memcpy(m_buf + m_pos, &aNum, theTypeSize);
			m_pos += theTypeSize;
			return *this;
		}
		throw std::out_of_range("BufferWriter out of range");
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

	BufferReader(const BufferReader&) = delete;
	BufferReader& operator=(const BufferReader&) = delete;

	template<typename T, typename = std::enable_if_t< std::is_arithmetic_v<T> >>
	BufferReader& operator>>(T& aNum) {
		static constexpr size_type theTypeSize = sizeof(T);
		if (m_pos + theTypeSize < m_size) {
			memcpy(&aNum, m_buf + m_pos, theTypeSize);
			m_pos += theTypeSize;
			return *this;
		}
		throw std::out_of_range("BufferReader out of range");
	}

	BufferReader& operator>>(std::string& aString);
	BufferReader& operator>>(Storable& aStorable);

private:
	const char* m_buf;
	size_type m_pos;
	size_type m_size;
};

}
#endif