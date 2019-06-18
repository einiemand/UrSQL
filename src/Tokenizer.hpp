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

	inline size_type remaining() const {
		return m_tokens.size() - m_index;
	}

	const Token& peek(size_type anOffset = 0);
	const Token& get();
	bool next(size_type anOffset = 1);

	bool skipIf(TokenType aType);
	bool skipIf(Keyword aKeyword);

	StatusResult tokenize();

private:
	std::istream& m_input;
	std::vector<Token> m_tokens;
	size_type m_index;

	std::string _readWhile(TokenizeCondition aCondition);
	std::string _readUntil(TokenizeCondition aCondition);
	inline std::string _readUntil(char aChar) {
		return _readUntil([aChar](char ch)->bool { return ch == aChar; });
	}
};

} /* UrSQL */

#endif /* Tokenizer.hpp */
