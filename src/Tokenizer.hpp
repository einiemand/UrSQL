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
	using TokenizeCondition = std::function<bool(char)>;

	explicit Tokenizer(std::istream& anInput);
	~Tokenizer() = default;

	Tokenizer(const Tokenizer&) = delete;
	Tokenizer& operator=(const Tokenizer&) = delete;

	inline bool more() const {
		return m_index < m_tokens.size();
	}

	inline size_type size() const {
		return m_tokens.size();
	}

	Token& peek();
	Token& get();

	StatusResult tokenize();

	std::string read_while(TokenizeCondition aCondition);
	std::string read_until(TokenizeCondition aCondition);
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
