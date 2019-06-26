#include "SQLStatement.hpp"
#include "Tokenizer.hpp"
#include "Attribute.hpp"
#include "Database.hpp"
#include "SQLInterpreter.hpp"
#include <unordered_set>

namespace UrSQL {

SQLStatement::SQLStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
	Statement(aTokenizer),
	m_interpreter(anInterpreter),
	m_name()
{
}

StatusResult SQLStatement::_parseTableName() {
	StatusResult theResult(Error::no_error);
	const Token& theNameToken = m_tokenizer.get();
	if (theNameToken.getType() == TokenType::identifier) {
		m_name = theNameToken.getData();
	}
	else {
		theResult.setError(Error::identifier_expected, "Table name is not an identifier");
	}
	return theResult;
}


/* -------------------------------CreateTableStatement------------------------------- */
class CreateTableStatement : public SQLStatement {
public:
	CreateTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
		SQLStatement(aTokenizer, anInterpreter)
	{
	}

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next(2)) {
			if (theResult = _parseTableName()) {
				if (m_tokenizer.skipIf(TokenType::lparen)) {
					if (theResult = _parseAttributeList()) {
						if (m_tokenizer.skipIf(TokenType::rparen)) {
							// success
						}
						else {
							theResult.setError(Error::syntax_error, "Use ')' to terminate attribute list");
						}
					}
				}
				else {
					theResult.setError(Error::syntax_error, "'(' missing after table name '" + m_name + '\'');
				}
			}
		}
		else {
			theResult.setError(Error::identifier_expected, "Table name unspecified");
		}
		return theResult;
	}

	StatusResult validate() const override {
		StatusResult theResult(Error::no_error);
		if (!m_tokenizer.more()) {
			theResult = _validateAttributes();
		}
		else {
			theResult.setError(Error::invalid_command, "Redundant input after ')'");
		}
		return theResult;
	}

	StatusResult execute() const override {
		StatusResult theResult(Error::no_error);
		if (Database* theActiveDB = m_interpreter.getActiveDatabase()) {

		}
		else {
			theResult.setError(Error::unknown_database, "Specify the database first by 'use <DBName>'");
		}
		return theResult;
	}

	~CreateTableStatement() override = default;
private:
	std::vector<AttributeBuilder> m_builders;

	StatusResult _parseAttributeName(AttributeBuilder& aBuilder) {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.more()) {
			const Token& theNameToken = m_tokenizer.get();
			if (theNameToken.getType() == TokenType::identifier) {
				aBuilder.setName(theNameToken.getData());
			}
			else {
				theResult.setError(Error::identifier_expected, "Attribute name is not an identifier");
			}
		}
		else {
			theResult.setError(Error::identifier_expected, "Attribute name missing");
		}
		return theResult;
	}

	StatusResult _parseAttributeType(AttributeBuilder& aBuilder) {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.more()) {
			const Token& theTypeToken = m_tokenizer.get();
			Keyword theKeyword = theTypeToken.getKeyword();
			if (Value::keywordIsValueType(theKeyword)) {
				aBuilder.setType(Value::keyword2ValueType(theKeyword));
			}
			else {
				theResult.setError(Error::syntax_error, "Unknown value type '" + theTypeToken.getData() + '\'');
			}
		}
		else {
			theResult.setError(Error::syntax_error, "Attribute type missing");
		}
		return theResult;
	}

	StatusResult _parseAttributeOptions(AttributeBuilder& aBuilder) {
		StatusResult theResult(Error::no_error);
		while (theResult && m_tokenizer.more() && m_tokenizer.peek().getType() != TokenType::comma) {
			const Token& theToken = m_tokenizer.get();
			switch (theToken.getKeyword()) {
			case Keyword::auto_increment_kw:
				aBuilder.setAutoIncr(true);
				break;
			case Keyword::primary_kw: {
				if (m_tokenizer.skipIf(Keyword::key_kw)) {
					aBuilder.setPrimary(true);
				}
				else {
					theResult.setError(Error::syntax_error, "Expect 'key' after 'primary'");
				}
				break;
			}
			case Keyword::not_kw: {
				if (m_tokenizer.skipIf(Keyword::null_kw)) {
					aBuilder.setNullable(false);
				}
				else {
					theResult.setError(Error::syntax_error, "Expect 'null' after 'not'");
				}
				break;
			}
			case Keyword::default_kw:
				theResult = _parseDefaultValue(aBuilder);
				break;
			default:
				theResult.setError(Error::syntax_error, "Invalid attribute option '" + theToken.getData() + '\'');
			}
		}
		return theResult;
	}

	StatusResult _parseAttribute(AttributeBuilder& aBuilder) {
		StatusResult theResult = _parseAttributeName(aBuilder);
		if (theResult) {
			theResult = _parseAttributeType(aBuilder);
			if (theResult) {
				theResult = _parseAttributeOptions(aBuilder);
			}
		}
		return theResult;
	}

	StatusResult _parseAttributeList() {
		StatusResult theResult(Error::no_error);
		do {
			AttributeBuilder theBuilder;
			if (theResult = _parseAttribute(theBuilder)) {
				m_builders.emplace_back(std::move(theBuilder));
			}
		} while (theResult && m_tokenizer.skipIf(TokenType::comma));

		return theResult;
	}

	StatusResult _parseDefaultValue(AttributeBuilder& aBuilder) {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.more()) {
			const Token& theValueToken = m_tokenizer.get();
			TokenType theType = theValueToken.getType();
			if (theType == TokenType::string || theType == TokenType::number) {
				Value theValue(theValueToken.getData());
				theResult = theValue.become(aBuilder.getType());
				if (theResult) {
					aBuilder.setDefaultValue(std::move(theValue));
				}
			}
			else if (theType == TokenType::keyword && theValueToken.getKeyword() == Keyword::null_kw) {
				aBuilder.setDefaultValue(Value{});
			}
			else {
				theResult.setError(Error::syntax_error, "Invalid default value '" + theValueToken.getData() + '\'');
			}
		}
		else {
			theResult.setError(Error::syntax_error, "Default value unspecified");
		}
		return theResult;
	}

	StatusResult _validateAttributes() const {
		std::unordered_set<std::string> theAttrNames;
		for (const auto& theBuilder : m_builders) {
			const std::string& theName = theBuilder.getName();
			if (theAttrNames.count(theName)) {
				return StatusResult(Error::invalid_attribute, '\'' + theName + "' redefinition");
			}
		}
		return StatusResult(Error::no_error);
	}

};

}