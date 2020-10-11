#include "model/DBStatement.hpp"

#include "controller/DBManager.hpp"
#include "model/Tokenizer.hpp"

namespace UrSQL {

DBStatement::DBStatement(Tokenizer& aTokenizer, DBManager& aDBManager)
    : Statement(aTokenizer),
      m_manager(aDBManager) {}

/* -------------------------------CreateDBStatement-------------------------------
 */
class CreateDBStatement : public DBStatement {
public:
    CreateDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager)
        : DBStatement(aTokenizer, aDBManager) {}

    ~CreateDBStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next(2)) {
            const Token& theToken = m_tokenizer.get();
            switch (theToken.getType()) {
            case TokenType::identifier:
                m_dbName = theToken.getData();
                break;
            case TokenType::string:
                theResult.setError(Error::syntax_error,
                                   "Don't surround identifiers with quotes");
                break;
            default:
                theResult.setError(
                  Error::unexpected_identifier,
                  "Illegal database name '" + theToken.getData() + '\'');
            }
        } else {
            theResult.setError(Error::identifier_expected,
                               "Database name expected");
        }
        return theResult;
    }

    StatusResult validate() const override {
        StatusResult theResult(Error::no_error);
        if (!m_tokenizer.more()) {
            bool exists = false;
            theResult = DBManager::databaseExists(exists, m_dbName);
            if (theResult && exists) {
                theResult.setError(Error::database_exists,
                                   '\'' + m_dbName + '\'');
            }
        } else {
            theResult.setError(Error::invalid_command,
                               "Redundant input after '" + m_dbName + '\'');
        }
        return theResult;
    }

    StatusResult execute() const override {
        return m_manager.createDatabase(m_dbName);
    }

private:
    std::string m_dbName;
};

/* -------------------------------DropDBStatement-------------------------------
 */
class DropDBStatement : public DBStatement {
public:
    DropDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager)
        : DBStatement(aTokenizer, aDBManager) {}

    ~DropDBStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next(2)) {
            const Token& theToken = m_tokenizer.get();
            switch (theToken.getType()) {
            case TokenType::identifier:
                m_dbName = theToken.getData();
                break;
            case TokenType::string:
                theResult.setError(Error::syntax_error,
                                   "Don't surround identifiers with quotes");
                break;
            default:
                theResult.setError(
                  Error::unexpected_identifier,
                  "Illegal database name '" + theToken.getData() + '\'');
            }
        } else {
            theResult.setError(Error::identifier_expected,
                               "Database name expected");
        }
        return theResult;
    }

    StatusResult validate() const override {
        StatusResult theResult(Error::no_error);
        if (!m_tokenizer.more()) {
            bool exists = false;
            theResult = DBManager::databaseExists(exists, m_dbName);
            if (theResult && !exists) {
                theResult.setError(Error::unknown_database,
                                   '\'' + m_dbName + '\'');
            }
        } else {
            theResult.setError(Error::invalid_command,
                               "Redundant input after '" + m_dbName + '\'');
        }
        return theResult;
    }

    StatusResult execute() const override {
        return m_manager.dropDatabase(m_dbName);
    }

private:
    std::string m_dbName;
};

/* -------------------------------UseDBStatement-------------------------------
 */
class UseDBStatement : public DBStatement {
public:
    UseDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager)
        : DBStatement(aTokenizer, aDBManager) {}

    ~UseDBStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next()) {
            const Token& theToken = m_tokenizer.get();
            switch (theToken.getType()) {
            case TokenType::identifier:
                m_dbName = theToken.getData();
                break;
            case TokenType::string:
                theResult.setError(Error::syntax_error,
                                   "Don't surround identifiers with quotes");
                break;
            default:
                theResult.setError(
                  Error::unexpected_identifier,
                  "Illegal database name '" + theToken.getData() + '\'');
            }
        } else {
            theResult.setError(Error::identifier_expected,
                               "Database name expected");
        }
        return theResult;
    }

    StatusResult validate() const override {
        StatusResult theResult(Error::no_error);
        if (!m_tokenizer.more()) {
            bool exists = false;
            theResult = DBManager::databaseExists(exists, m_dbName);
            if (theResult && !exists) {
                theResult.setError(Error::unknown_database,
                                   '\'' + m_dbName + '\'');
            }
        } else {
            theResult.setError(Error::invalid_command,
                               "Redundant input after '" + m_dbName + '\'');
        }
        return theResult;
    }

    StatusResult execute() const override {
        return m_manager.useDatabase(m_dbName);
    }

private:
    std::string m_dbName;
};

/* -------------------------------ShowDBStatement-------------------------------
 */
class ShowDBStatement : public DBStatement {
public:
    ShowDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager)
        : DBStatement(aTokenizer, aDBManager) {}

    ~ShowDBStatement() override = default;

    StatusResult parse() override {
        m_tokenizer.next(2);
        return StatusResult(Error::no_error);
    }

    StatusResult validate() const override {
        return !m_tokenizer.more() ?
                 StatusResult(Error::no_error) :
                 StatusResult(Error::invalid_command,
                              "Redundant input after 'databases'");
    }

    StatusResult execute() const override {
        return m_manager.showDatabases();
    }
};

/* -------------------------------DescDBStatement-------------------------------
 */
class DescDBStatement : public DBStatement {
public:
    DescDBStatement(Tokenizer& aTokenizer, DBManager& aDBManager)
        : DBStatement(aTokenizer, aDBManager) {}

    ~DescDBStatement() override = default;

    StatusResult parse() override {
        StatusResult theResult(Error::no_error);
        if (m_tokenizer.next(2)) {
            const Token& theToken = m_tokenizer.get();
            switch (theToken.getType()) {
            case TokenType::identifier:
                m_dbName = theToken.getData();
                break;
            case TokenType::string:
                theResult.setError(Error::syntax_error,
                                   "Don't surround identifiers with quotes");
                break;
            default:
                theResult.setError(
                  Error::unexpected_identifier,
                  "Illegal database name '" + theToken.getData() + '\'');
            }
        } else {
            theResult.setError(Error::identifier_expected,
                               "Database name expected");
        }
        return theResult;
    }

    StatusResult validate() const override {
        StatusResult theResult(Error::no_error);
        if (!m_tokenizer.more()) {
            bool exists = false;
            theResult = DBManager::databaseExists(exists, m_dbName);
            if (theResult && !exists) {
                theResult.setError(Error::unknown_database,
                                   '\'' + m_dbName + '\'');
            }
        } else {
            theResult.setError(Error::invalid_command,
                               "Redundant input after '" + m_dbName + '\'');
        }
        return theResult;
    }

    StatusResult execute() const override {
        return m_manager.describeDatabase(m_dbName);
    }

private:
    std::string m_dbName;
};

/* -------------------------------Static DBStatement
 * Methods------------------------------- */
std::unique_ptr<DBStatement> DBStatement::factory(Tokenizer& aTokenizer,
                                                  DBManager& aDBManager) {
    Keyword theKeyword = aTokenizer.peek().getKeyword();
    switch (theKeyword) {
    case Keyword::create_kw:
        return std::make_unique<CreateDBStatement>(aTokenizer, aDBManager);
    case Keyword::drop_kw:
        return std::make_unique<DropDBStatement>(aTokenizer, aDBManager);
    case Keyword::use_kw:
        return std::make_unique<UseDBStatement>(aTokenizer, aDBManager);
    case Keyword::show_kw:
        return std::make_unique<ShowDBStatement>(aTokenizer, aDBManager);
    case Keyword::describe_kw:
    case Keyword::desc_kw:
        return std::make_unique<DescDBStatement>(aTokenizer, aDBManager);
    default:
        return nullptr;
    }
}

}  // namespace UrSQL
