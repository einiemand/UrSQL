#pragma once
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <istream>
#include <vector>
#include <functional>

namespace UrSQL {

using size_type = std::size_t;

class Token;

class Tokenizer {
public:
	using tokenize_condition = std::function<bool(char)>;

	Tokenizer(std::istream& anInput);
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

	void tokenize();

	std::string read_while(tokenize_condition aCondition);
	std::string read_until(tokenize_condition aCondition);

private:
	std::istream& m_input;
	std::vector<Token> m_tokens;
	size_type m_index;
};

} /* UrSQL */

#endif /* Tokenizer.hpp */
