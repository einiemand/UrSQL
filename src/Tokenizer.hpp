#pragma once
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include "Token.hpp"
#include "Error.hpp"
#include <istream>
#include <vector>
#include <functional>

namespace UrSQL {

class Tokenizer {
public:
	using tokenize_condition = std::function<bool(char)>;

	explicit Tokenizer(std::istream& anInput);
	~Tokenizer() = default;

	Tokenizer(const Tokenizer&) = delete;
	Tokenizer& operator=(const Tokenizer&) = delete;

	inline bool more() const {
		return m_index < m_tokens.size();
	}

	inline void next(size_type anOffset = 1) {
		m_index += anOffset;
	}

	inline Token& current() {
		return m_tokens[m_index];
	}

	StatusResult tokenize();

	std::string read_while(tokenize_condition aCondition);
	std::string read_until(tokenize_condition aCondition);
	inline std::string read_until(char aChar) {
		return read_until([aChar](char ch)->bool { return ch == aChar; });
	}

private:
	std::istream& m_input;
	std::vector<Token> m_tokens;
	size_type m_index;
};

} /* UrSQL */

#endif /* Tokenizer.hpp */
