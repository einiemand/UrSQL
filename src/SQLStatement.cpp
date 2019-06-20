#include "SQLStatement.hpp"
#include "Tokenizer.hpp"
#include "Attribute.hpp"

namespace UrSQL {

SQLStatement::SQLStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
	Statement(aTokenizer),
	m_interpreter(anInterpreter),
	m_name()
{
}

StatusResult SQLStatement::parseTableName() {
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
			theResult = parseTableName();
			if (theResult) {
				if (m_tokenizer.skipIf(TokenType::lparen)) {
					AttributeBuilder theBuilder;
					theResult = parseAttributeName(theBuilder);
					if (theResult) {
						theResult = parseAttributeType(theBuilder);
						if (theResult) {

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

	StatusResult validate() const override;
	StatusResult execute() const override;

	~CreateTableStatement() override = default;
private:
	std::vector<AttributeBuilder> m_builders;

	StatusResult parseAttributeName(AttributeBuilder& aBuilder) {
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

	StatusResult parseAttributeType(AttributeBuilder& aBuilder) {
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
};

}