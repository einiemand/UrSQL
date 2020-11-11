#include "model/SQLStatement.hpp"

#include <unordered_set>

#include "controller/SQLInterpreter.hpp"
#include "model/Attribute.hpp"
#include "model/Database.hpp"
#include "model/Filter.hpp"
#include "model/Order.hpp"
#include "model/Row.hpp"
#include "model/Tokenizer.hpp"

namespace UrSQL {

SQLStatement::SQLStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
    : Statement(aTokenizer),
      m_interpreter(anInterpreter),
      m_entityName() {}

StatusResult SQLStatement::_parseTableName() {
    StatusResult theResult(Error::no_error);
    if (m_tokenizer.more()) {
        const Token& theNameToken = m_tokenizer.get();
        if (theNameToken.getType() == TokenType::identifier) {
            m_entityName = theNameToken.getData();
        }
        else {
            theResult.setError(Error::identifier_expected,
                               "Table name is not an identifier");
        }
    }
    else {
        theResult.setError(Error::identifier_expected, "Table name missing");
    }
    return theResult;
}

/* -------------------------------Helper
 * Functions------------------------------- */
using TokenFilter = std::function<bool(const Token&)>;

StatusResult parseSequence(Tokenizer& aTokenizer, StringList& aContainer,
                           TokenFilter aFilter) {
    StatusResult theResult(Error::no_error);
    if (aTokenizer.more()) {
        do {
            const Token& theToken = aTokenizer.get();
            if (aFilter(theToken)) {
                aContainer.emplace_back(theToken.getData());
            }
            else {
                theResult.setError(
                  Error::syntax_error,
                  "Invalid input '" + theToken.getData() + '\'');
            }
        } while (theResult && aTokenizer.skipIf(TokenType::comma));
    }
    else {
        theResult.setError(Error::syntax_error, "Input not enough");
    }
    return theResult;
}

template<typename ElemT, typename CompT>
std::string examineOverlap(const std::vector<ElemT>& aContainer,
                           std::function<CompT(const ElemT&)> anExtracter) {
    std::unordered_set<CompT> theCompElemSet;
    for (const auto& theElem : aContainer) {
        CompT theCompElem = anExtracter(theElem);
        if (theCompElemSet.count(theCompElem)) {
            return theCompElem;
        }
    }
    return "";
}

/* -------------------------------CreateTableStatement-------------------------------
 */
class CreateTableStatement : public SQLStatement {
public:
    CreateTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter) {}

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next(2)) {
            theResult = _parseTableName();
            if (theResult) {
                if (m_tokenizer.skipIf(TokenType::lparen)) {
                    theResult = _parseAttributeList();
                    if (theResult) {
                        if (m_tokenizer.skipIf(TokenType::rparen)) {
                            // success
                        }
                        else {
                            theResult.setError(
                              Error::syntax_error,
                              "Use ')' to terminate attribute list");
                        }
                    }
                }
                else {
                    theResult.setError(
                      Error::syntax_error,
                      "'(' missing after table name '" + m_entityName + '\'');
                }
            }
        }
        else {
            theResult.setError(Error::identifier_expected,
                               "Table name unspecified");
        }
        return theResult;
    }

    StatusResult validate() const override {
        StatusResult theResult(Error::no_error);
        if (!m_tokenizer.more()) {
            theResult = _validateAttributes();
        }
        else {
            theResult.setError(Error::invalid_command,
                               "Redundant input after ')'");
        }
        return theResult;
    }

    StatusResult execute() const override {
        return m_interpreter.createTable(m_attributes, m_entityName);
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
                theResult.setError(Error::identifier_expected,
                                   "Attribute name is not an identifier");
            }
        }
        else {
            theResult.setError(Error::identifier_expected,
                               "Attribute name missing");
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
                theResult.setError(
                  Error::syntax_error,
                  "Unknown value type '" + theTypeToken.getData() + '\'');
            }
        }
        else {
            theResult.setError(Error::syntax_error, "Attribute type missing");
        }
        return theResult;
    }

    StatusResult _parseAttributeOptions(Attribute& anAttribute) {
        StatusResult theResult(Error::no_error);
        while (theResult && m_tokenizer.more() &&
               CreateTableStatement::_isValidAttributeOption(
                 m_tokenizer.peek().getKeyword()))
        {
            const Token& theToken = m_tokenizer.get();
            Keyword theKeyword = theToken.getKeyword();
            switch (theKeyword) {
            case Keyword::auto_increment_kw:
                anAttribute.setAutoIncr(true);
                break;
            case Keyword::primary_kw: {
                if (m_tokenizer.skipIf(Keyword::key_kw)) {
                    anAttribute.setPrimary(true);
                    anAttribute.setNullable(false);
                }
                else {
                    theResult.setError(Error::syntax_error,
                                       "Expect 'key' after 'primary'");
                }
                break;
            }
            case Keyword::not_kw: {
                if (m_tokenizer.skipIf(Keyword::null_kw)) {
                    anAttribute.setNullable(false);
                }
                else {
                    theResult.setError(Error::syntax_error,
                                       "Expect 'null' after 'not'");
                }
                break;
            }
            default:
                break;
            }
        }
        if (m_tokenizer.more()) {
            const Token& theToken = m_tokenizer.peek();
            TokenType theType = theToken.getType();
            if (theType != TokenType::comma && theType != TokenType::rparen) {
                theResult.setError(
                  Error::syntax_error,
                  "Invalid attribute option '" + theToken.getData() + '\'');
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
            theResult = _parseAttribute(theAttribute);
            if (theResult) {
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
            else if (theType == TokenType::keyword &&
                     theValueToken.getKeyword() == Keyword::null_kw)
            {
                anAttribute.setDefaultValue(Value{});
            }
            else {
                theResult.setError(
                  Error::syntax_error,
                  "Invalid default value '" + theValueToken.getData() + '\'');
            }
        }
        else {
            theResult.setError(Error::syntax_error,
                               "Default value unspecified");
        }
        return theResult;
    }

    StatusResult _validateAttributes() const {
        std::string theOverlap = examineOverlap<Attribute, std::string>(
          m_attributes, [](const Attribute& anAttribute) -> std::string {
              return anAttribute.getName();
          });
        return theOverlap.empty() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::invalid_arguments,
                              '\'' + theOverlap + "' redefined");
    }

    static bool _isValidAttributeOption(Keyword aKeyword) {
        return aKeyword == Keyword::primary_kw ||
               aKeyword == Keyword::auto_increment_kw ||
               aKeyword == Keyword::not_kw;
    }
};

/* -------------------------------DescTableStatement-------------------------------
 */
class DescTableStatement : public SQLStatement {
public:
    DescTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter) {}
    ~DescTableStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next(2)) {
            const Token& theNameToken = m_tokenizer.get();
            if (theNameToken.getType() == TokenType::identifier) {
                m_entityName = theNameToken.getData();
            }
            else {
                theResult.setError(
                  Error::unexpected_identifier,
                  "'" + theNameToken.getData() + "' is not a valid table name");
            }
        }
        else {
            theResult.setError(Error::identifier_expected,
                               "Table name unspecified");
        }
        return theResult;
    }

    StatusResult validate() const override {
        return m_tokenizer.more() ?
                 StatusResult(Error::invalid_command,
                              "Redundant input after '" + m_entityName + '\'') :
                 StatusResult(Error::no_error);
    }

    StatusResult execute() const override {
        return m_interpreter.describeTable(m_entityName);
    }
};

/* -------------------------------InsertStatement-------------------------------
 */
class InsertStatement : public SQLStatement {
public:
    InsertStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter) {}

    ~InsertStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next() && m_tokenizer.skipIf(Keyword::into_kw)) {
            theResult = _parseTableName();
            if (theResult) {
                theResult = _parseFieldNames();
                if (theResult) {
                    if (m_tokenizer.skipIf(Keyword::values_kw)) {
                        theResult = _parseMultipleRows();
                    }
                    else {
                        theResult.setError(Error::syntax_error,
                                           "'values' missing");
                    }
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
            if (std::all_of(m_valueStrs.cbegin(), m_valueStrs.cend(),
                            [this](const StringList& aRowValueStrs) {
                                return aRowValueStrs.size() ==
                                       m_fieldNames.size();
                            }))
            {
                theResult = _validateFieldNames();
            }
            else {
                theResult.setError(
                  Error::keyValue_mismatch,
                  "The number of fields is not equal to the number of values");
            }
        }
        else {
            theResult.setError(Error::syntax_error,
                               "Redundant input after ')'");
        }
        return theResult;
    }

    StatusResult execute() const override {
        return m_interpreter.insertIntoTable(m_entityName, m_fieldNames,
                                             m_valueStrs);
    }

private:
    StringList m_fieldNames;
    std::vector<StringList> m_valueStrs;

    StatusResult _parseFieldNames() {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.skipIf(TokenType::lparen)) {
            theResult = parseSequence(
              m_tokenizer, m_fieldNames, [](const auto& aToken) -> bool {
                  return aToken.getType() == TokenType::identifier;
              });
            if (theResult) {
                if (m_tokenizer.skipIf(TokenType::rparen)) {
                    // success
                }
                else {
                    theResult.setError(Error::syntax_error, "')' missing");
                }
            }
        }
        else {
            theResult.setError(
              Error::syntax_error,
              "'(' missing after table name '" + m_entityName + "'");
        }
        return theResult;
    }

    StatusResult _parseMultipleRows() {
        StatusResult theResult(Error::no_error);
        do {
            theResult = _parseOneRow();
        } while (theResult && m_tokenizer.skipIf(TokenType::comma));
        return theResult;
    }

    StatusResult _parseOneRow() {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.skipIf(TokenType::lparen)) {
            m_valueStrs.emplace_back();
            theResult = parseSequence(m_tokenizer, m_valueStrs.back(),
                                      [](const Token& aToken) {
                                          return aToken.isValue();
                                      });
            if (theResult) {
                if (m_tokenizer.skipIf(TokenType::rparen)) {
                    //  success
                }
                else {
                    theResult.setError(Error::syntax_error, "')' missing");
                }
            }
        }
        else {
            theResult.setError(Error::syntax_error,
                               "'(' missing after 'values' or ','");
        }
        return theResult;
    }

    StatusResult _validateFieldNames() const {
        std::string theOverlap = examineOverlap<std::string, std::string>(
          m_fieldNames, [](const std::string& aFieldName) -> std::string {
              return aFieldName;
          });
        return theOverlap.empty() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::invalid_arguments,
                              '\'' + theOverlap + "' redefined");
    }
};

/* -------------------------------SelectStatement-------------------------------
 */
class SelectStatement : public SQLStatement {
public:
    SelectStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter),
          m_fieldNames(),
          m_filter(nullptr),
          m_order(nullptr) {}

    ~SelectStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next()) {
            if (!m_tokenizer.skipIf(TokenType::star)) {
                theResult = parseSequence(
                  m_tokenizer, m_fieldNames, [](const Token& aToken) -> bool {
                      return aToken.getType() == TokenType::identifier;
                  });
            }
            if (theResult) {
                if (m_tokenizer.skipIf(Keyword::from_kw)) {
                    theResult = _parseTableName();
                    if (theResult && m_tokenizer.skipIf(Keyword::where_kw)) {
                        theResult = _parseFilter();
                    }
                    if (theResult && m_tokenizer.skipIf(Keyword::order_kw)) {
                        theResult = _parseOrderBy();
                    }
                }
                else {
                    theResult.setError(Error::keyword_expected, "'from'");
                }
            }
        }
        else {
            theResult.setError(Error::identifier_expected,
                               "Specify from which fields");
        }
        return theResult;
    }

    StatusResult validate() const override {
        return !m_tokenizer.more() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::syntax_error,
                              "Redundant input after '" + m_entityName + '\'');
    }

    StatusResult execute() const override {
        return m_interpreter.selectFromTable(m_entityName, m_fieldNames,
                                             m_filter.get(), m_order.get());
    }

private:
    mutable StringList m_fieldNames;
    std::unique_ptr<Filter> m_filter;
    std::unique_ptr<Order> m_order;

    StatusResult _parseFilter() {
        m_filter = std::make_unique<Filter>();
        return m_filter->parse(m_tokenizer);
    }

    StatusResult _parseOrderBy() {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.skipIf(Keyword::by_kw)) {
            if (m_tokenizer.more()) {
                const Token& theToken = m_tokenizer.get();
                if (theToken.getType() == TokenType::identifier) {
                    m_order = std::make_unique<Order>();
                    m_order->setFieldName(theToken.getData());
                    if (m_tokenizer.skipIf(Keyword::desc_kw)) {
                        m_order->setDesc(true);
                    }
                    else if (m_tokenizer.skipIf(Keyword::asc_kw)) {
                        // do nothing
                    }
                }
                else {
                    theResult.setError(
                      Error::identifier_expected,
                      '\'' + theToken.getData() + "' is not an identifier");
                }
            }
            else {
                theResult.setError(Error::identifier_expected,
                                   "Specify the field name to be ordered by");
            }
        }
        else {
            theResult.setError(Error::keyword_expected, "'by'");
        }
        return theResult;
    }
};

/* -------------------------------DropTableStatement-------------------------------
 */
class DropTableStatement : public SQLStatement {
public:
    DropTableStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter) {}

    ~DropTableStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next(2)) {
            theResult = _parseTableName();
        }
        else {
            theResult.setError(Error::identifier_expected,
                               "Table name unspecified");
        }
        return theResult;
    }

    StatusResult validate() const override {
        return !m_tokenizer.more() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::syntax_error,
                              "Redundant input after '" + m_entityName + '\'');
    }

    StatusResult execute() const override {
        return m_interpreter.dropTable(m_entityName);
    }
};

/* -------------------------------ShowTablesStatement-------------------------------
 */
class ShowTablesStatement : public SQLStatement {
public:
    ShowTablesStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter) {}

    ~ShowTablesStatement() override = default;

    StatusResult parse() override {
        m_tokenizer.next(2);
        return StatusResult(Error::no_error);
    }

    StatusResult validate() const override {
        return !m_tokenizer.more() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::syntax_error,
                              "Redundant input after '" + m_entityName + '\'');
    }

    StatusResult execute() const override {
        return m_interpreter.showTables();
    }
};

/* -------------------------------DeleteStatement-------------------------------
 */
class DeleteStatement : public SQLStatement {
public:
    DeleteStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter),
          m_filter(nullptr) {}

    ~DeleteStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next() && m_tokenizer.skipIf(Keyword::from_kw)) {
            theResult = _parseTableName();
            if (theResult) {
                if (m_tokenizer.skipIf(Keyword::where_kw)) {
                    m_filter = std::make_unique<Filter>();
                    theResult = m_filter->parse(m_tokenizer);
                }
                else {
                    theResult.setError(Error::keyword_expected, "'where'");
                }
            }
        }
        else {
            theResult.setError(Error::keyword_expected, "'from'");
        }
        return theResult;
    }

    StatusResult validate() const override {
        return !m_tokenizer.more() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::syntax_error,
                              "Redundant input after filter");
    }

    StatusResult execute() const override {
        return m_interpreter.deleteFromTable(m_entityName, m_filter.get());
    }

private:
    std::unique_ptr<Filter> m_filter;
};

/* -------------------------------UpdateStatement-------------------------------
 */
class UpdateStatement : public SQLStatement {
public:
    UpdateStatement(Tokenizer& aTokenizer, SQLInterpreter& anInterpreter)
        : SQLStatement(aTokenizer, anInterpreter),
          m_fieldMap(),
          m_filter(nullptr) {}

    ~UpdateStatement() override = default;

    StatusResult parse() override {
        m_tokenizer.next();
        StatusResult theResult = _parseTableName();
        if (m_tokenizer.skipIf(Keyword::set_kw)) {
            theResult = _parseFieldValues();
            if (theResult && m_tokenizer.skipIf(Keyword::where_kw)) {
                m_filter = std::make_unique<Filter>();
                theResult = m_filter->parse(m_tokenizer);
            }
        }
        else {
            theResult.setError(Error::keyword_expected, "'set'");
        }
        return theResult;
    }

    StatusResult validate() const override {
        return !m_tokenizer.more() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::syntax_error, "Redundant input");
    }

    StatusResult execute() const override {
        return m_interpreter.updateTable(m_entityName, m_fieldMap,
                                         m_filter.get());
    }

private:
    Row::DataMap m_fieldMap;
    std::unique_ptr<Filter> m_filter;

    StatusResult _parseFieldValues() {
        StatusResult theResult(Error::no_error);
        do {
            theResult = _parseOneFieldValue();
        } while (theResult && m_tokenizer.skipIf(TokenType::comma));
        return theResult;
    }

    StatusResult _parseOneFieldValue() {
        std::string theFieldName;
        StatusResult theResult = _parseFieldName(theFieldName);
        if (theResult) {
            if (m_tokenizer.skipIf([](const Token& aToken) {
                    return aToken.getData() == "=";
                }))
            {
                Value theValue;
                theResult = _parseValue(theValue);
                if (theResult) {
                    m_fieldMap.insert(
                      { std::move(theFieldName), std::move(theValue) });
                }
            }
            else {
                theResult.setError(Error::syntax_error,
                                   "Missing '=' after field name");
            }
        }
        return theResult;
    }

    StatusResult _parseFieldName(std::string& aFieldName) {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.more()) {
            const Token& theFieldToken = m_tokenizer.get();
            if (theFieldToken.getType() == TokenType::identifier) {
                const std::string& theFieldName = theFieldToken.getData();
                if (!m_fieldMap.count(theFieldName)) {
                    aFieldName = theFieldName;
                }
                else {
                    theResult.setError(Error::keyValue_mismatch,
                                       '\'' + aFieldName + "' are repeating");
                }
            }
            else {
                theResult.setError(Error::identifier_expected,
                                   "Which fields are you trying to modify?");
            }
        }
        else {
            theResult.setError(Error::syntax_error,
                               "Fields and values missing");
        }
        return theResult;
    }

    StatusResult _parseValue(Value& aValue) {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.more()) {
            const Token& theValueToken = m_tokenizer.get();
            if (theValueToken.isValue()) {
                const std::string& theTokenData = theValueToken.getData();
                aValue = theTokenData;
                if (theValueToken.getType() == TokenType::number) {
                    if (std::any_of(theTokenData.cbegin(), theTokenData.cend(),
                                    [](char aChar) {
                                        return aChar == '.';
                                    }))
                    {
                        theResult = aValue.become(ValueType::float_type);
                    }
                    else {
                        theResult = aValue.become(ValueType::int_type);
                    }
                }
            }
            else {
                theResult.setError(Error::syntax_error,
                                   '\'' + theValueToken.getData() +
                                     "' is neither a string nor a number");
            }
        }
        else {
            theResult.setError(Error::value_expected, "What value to assign??");
        }
        return theResult;
    }
};

// factory function
std::unique_ptr<SQLStatement> SQLStatement::factory(
  Tokenizer& aTokenizer, SQLInterpreter& anInterpreter) {
    Keyword theKeyword = aTokenizer.peek().getKeyword();
    switch (theKeyword) {
    case Keyword::create_kw:
        return std::make_unique<CreateTableStatement>(aTokenizer,
                                                      anInterpreter);
    case Keyword::describe_kw:
    case Keyword::desc_kw:
        return std::make_unique<DescTableStatement>(aTokenizer, anInterpreter);
    case Keyword::insert_kw:
        return std::make_unique<InsertStatement>(aTokenizer, anInterpreter);
    case Keyword::select_kw:
        return std::make_unique<SelectStatement>(aTokenizer, anInterpreter);
    case Keyword::drop_kw:
        return std::make_unique<DropTableStatement>(aTokenizer, anInterpreter);
    case Keyword::show_kw:
        return std::make_unique<ShowTablesStatement>(aTokenizer, anInterpreter);
    case Keyword::delete_kw:
        return std::make_unique<DeleteStatement>(aTokenizer, anInterpreter);
    case Keyword::update_kw:
        return std::make_unique<UpdateStatement>(aTokenizer, anInterpreter);
    default:
        return nullptr;
    }
}

}  // namespace UrSQL
