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
	if (m_tokenizer.more()) {
		const Token& theNameToken = m_tokenizer.get();
		if (theNameToken.getType() == TokenType::identifier) {
			m_name = theNameToken.getData();
		}
		else {
			theResult.setError(Error::identifier_expected, "Table name is not an identifier");
		}
	}
	else {
		theResult.setError(Error::identifier_expected, "Table name missing");
	}
	return theResult;
}

/* -------------------------------Helper Functions------------------------------- */
using TokenFilter = std::function<bool(const Token&)>;

StatusResult parseSequence(Tokenizer& aTokenizer, StringList& aContainer, TokenFilter aFilter) {
	StatusResult theResult(Error::no_error);
	if (aTokenizer.more()) {
		do {
			const Token& theToken = aTokenizer.get();
			if (aFilter(theToken)) {
				aContainer.emplace_back(theToken.getData());
			}
			else {
				theResult.setError(Error::syntax_error, "Invalid input '" + theToken.getData() + '\'');
			}
		} while (theResult && aTokenizer.skipIf(TokenType::comma));
	}
	else {
		theResult.setError(Error::syntax_error, "Input not enough");
	}
	return theResult;
}

template<typename ElemT, typename CompT>
std::string examineOverlap(const std::vector<ElemT>& aContainer, std::function<CompT(const ElemT&)> anExtracter) {
	std::unordered_set<CompT> theCompElemSet;
	for (const auto& theElem : aContainer) {
		CompT theCompElem = anExtracter(theElem);
		if (theCompElemSet.count(theCompElem)) {
			return theCompElem;
		}
	}
	return "";
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
		return m_interpreter.createTable(m_attributes, m_name);
	}

	~CreateTableStatement() override = default;
private:
	AttributeList m_attributes;

	StatusResult _parseAttributeName(Attribute& anAttribute) {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.more()) {
			const Token& theNameToken = m_tokenizer.get();
			if (theNameToken.getType() == TokenType::identifier) {
				anAttribute.setName(theNameToken.getData());
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

	StatusResult _parseAttributeType(Attribute& anAttribute) {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.more()) {
			const Token& theTypeToken = m_tokenizer.get();
			Keyword theKeyword = theTypeToken.getKeyword();
			if (Value::keywordIsValueType(theKeyword)) {
				anAttribute.setType(Value::keyword2ValueType(theKeyword));
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

	StatusResult _parseAttributeOptions(Attribute& anAttribute) {
		StatusResult theResult(Error::no_error);
		while (theResult && m_tokenizer.more() && CreateTableStatement::_isValidAttributeOption(m_tokenizer.peek().getKeyword())) {
			const Token& theToken = m_tokenizer.get();
			Keyword theKeyword = theToken.getKeyword();
			switch (theKeyword) {
			case Keyword::auto_increment_kw:
				anAttribute.setAutoIncr(true);
				break;
			case Keyword::primary_kw: {
				if (m_tokenizer.skipIf(Keyword::key_kw)) {
					anAttribute.setPrimary(true);
				}
				else {
					theResult.setError(Error::syntax_error, "Expect 'key' after 'primary'");
				}
				break;
			}
			case Keyword::not_kw: {
				if (m_tokenizer.skipIf(Keyword::null_kw)) {
					anAttribute.setNullable(false);
				}
				else {
					theResult.setError(Error::syntax_error, "Expect 'null' after 'not'");
				}
				break;
			}
								  //case Keyword::default_kw:
								  //	theResult = _parseDefaultValue(anAttribute);
								  //	break;
			}
		}
		if (m_tokenizer.more()) {
			const Token& theToken = m_tokenizer.peek();
			TokenType theType = theToken.getType();
			if (theType != TokenType::comma && theType != TokenType::rparen) {
				theResult.setError(Error::syntax_error, "Invalid attribute option '" + theToken.getData() + '\'');
			}
		}
		return theResult;
	}

	StatusResult _parseAttribute(Attribute& anAttribute) {
		StatusResult theResult = _parseAttributeName(anAttribute);
		if (theResult) {
			theResult = _parseAttributeType(anAttribute);
			if (theResult) {
				theResult = _parseAttributeOptions(anAttribute);
			}
		}
		return theResult;
	}

	StatusResult _parseAttributeList() {
		StatusResult theResult(Error::no_error);
		do {
			Attribute theAttribute;
			if (theResult = _parseAttribute(theAttribute)) {
				m_attributes.emplace_back(std::move(theAttribute));
			}
		} while (theResult && m_tokenizer.skipIf(TokenType::comma));

		return theResult;
	}

	StatusResult _parseDefaultValue(Attribute& anAttribute) {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.more()) {
			const Token& theValueToken = m_tokenizer.get();
			TokenType theType = theValueToken.getType();
			if (theType == TokenType::string || theType == TokenType::number) {
				Value theValue(theValueToken.getData());
				theResult = theValue.become(anAttribute.getType());
				if (theResult) {
					anAttribute.setDefaultValue(std::move(theValue));
				}
			}
			else if (theType == TokenType::keyword && theValueToken.getKeyword() == Keyword::null_kw) {
				anAttribute.setDefaultValue(Value{});
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
		std::string theOverlap = examineOverlap<Attribute, std::string>(m_attributes,
			[](const Attribute& anAttribute)->std::string {
				return anAttribute.getName();
			}
		);
		return theOverlap.empty() ?
			StatusResult(Error::no_error) : StatusResult(Error::invalid_arguments, '\'' + theOverlap + "' redefined");
	}

	static bool _isValidAttributeOption(Keyword aKeyword) {
		return aKeyword == Keyword::primary_kw || aKeyword == Keyword::auto_increment_kw || aKeyword == Keyword::not_kw;
	}

};

/* -------------------------------DescTableStatement------------------------------- */
class DescTableStatement : public SQLStatement {
public:
	DescTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
		SQLStatement(aTokenizer, anInterpreter)
	{
	}
	~DescTableStatement() override = default;

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next(2)) {
			const Token& theNameToken = m_tokenizer.get();
			if (theNameToken.getType() == TokenType::identifier) {
				m_name = theNameToken.getData();
			}
			else {
				theResult.setError(Error::unexpected_identifier, "'" + theNameToken.getData() + "' is not a valid table name");
			}
		}
		else {
			theResult.setError(Error::identifier_expected, "Table name unspecified");
		}
		return theResult;
	}

	StatusResult validate() const override {
		return m_tokenizer.more() ?
			StatusResult(Error::invalid_command, "Redundant input after '" + m_name + '\'') : StatusResult(Error::no_error);
	}

	StatusResult execute() const override {
		return m_interpreter.describeTable(m_name);
	}
};

/* -------------------------------InsertIntoTableStatement------------------------------- */
class InsertIntoTableStatement : public SQLStatement {
public:
	InsertIntoTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) :
		SQLStatement(aTokenizer, anInterpreter)
	{
	}

	~InsertIntoTableStatement() override = default;

	StatusResult parse() override {
		StatusResult theResult(Error::no_error);
		if (m_tokenizer.next() && m_tokenizer.skipIf(Keyword::into_kw)) {
			if (theResult = _parseTableName()) {
				if (m_tokenizer.skipIf(TokenType::lparen)) {
					if (theResult = _parseFieldNames()) {
						if (m_tokenizer.skipIf(TokenType::rparen)) {
							if (m_tokenizer.skipIf(Keyword::values_kw)) {
								if (m_tokenizer.skipIf(TokenType::lparen)) {
									if (theResult = _parseValueStrings()) {
										if (m_tokenizer.skipIf(TokenType::rparen)) {
											//  success
										}
										else {
											theResult.setError(Error::syntax_error, "')' missing");
										}
									}
								}
								else {
									theResult.setError(Error::syntax_error, "'(' missing after 'values'");
								}
							}
							else {
								theResult.setError(Error::syntax_error, "'values' missing");
							}
						}
						else {
							theResult.setError(Error::syntax_error, "')' missing");
						}
					}
				}
				else {
					theResult.setError(Error::syntax_error, "'(' missing after table name '" + m_name + "'");
				}
			}
		}
		else {
			theResult.setError(Error::keyword_expected, "'into'");
		}
		return theResult;
	}

	StatusResult validate() const override {
		StatusResult theResult(Error::no_error);
		if (!m_tokenizer.more()) {
			if (m_fieldNames.size() != m_valueStrs.size()) {
				theResult = _validateFieldNames();
			}
			else {
				theResult.setError(Error::keyValue_mismatch, "The number of fields is not equal to the number of values");
			}
		}
		else {
			theResult.setError(Error::syntax_error, "Redundant input after ')'");
		}
		return theResult;
	}

	StatusResult execute() const override {
		return m_interpreter.insertIntoTable(m_name, m_fieldNames, m_valueStrs);
	}
private:
	StringList m_fieldNames;
	StringList m_valueStrs;

	inline StatusResult _parseFieldNames() {
		return parseSequence(m_tokenizer, m_fieldNames,
			[](const auto& aToken)->bool {
				return aToken.getType() == TokenType::identifier;
			}
		);
	}

	inline StatusResult _parseValueStrings() {
		return parseSequence(m_tokenizer, m_valueStrs,
			[](const auto& aToken)->bool {
				TokenType theType = aToken.getType();
				return theType == TokenType::string || theType == TokenType::number;
			}
		);
	}

	StatusResult _validateFieldNames() const {
		std::string theOverlap = examineOverlap<std::string, std::string>(m_fieldNames,
			[](const std::string& aFieldName)->std::string {
				return aFieldName;
			}
		);
		return theOverlap.empty() ?
			StatusResult(Error::no_error) : StatusResult(Error::invalid_arguments, '\'' + theOverlap + "' redefined");
	}
};

std::unique_ptr<SQLStatement> SQLStatement::factory(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) {
	Keyword theKeyword = aTokenizer.peek().getKeyword();
	switch (theKeyword) {
	case Keyword::create_kw:
		return std::make_unique<CreateTableStatement>(aTokenizer, anInterpreter);
	case Keyword::describe_kw:
		return std::make_unique<DescTableStatement>(aTokenizer, anInterpreter);
	case Keyword::insert_kw:
		return std::make_unique<InsertIntoTableStatement>(aTokenizer, anInterpreter);
	default:
		return nullptr;
	}
}

}