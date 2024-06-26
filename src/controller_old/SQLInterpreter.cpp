#include "controller/SQLInterpreter.hpp"

#include "model/Database.hpp"
#include "model/Filter.hpp"
#include "model/Order.hpp"
#include "statement/TableStatement.hpp"
#include "token/Tokenizer.hpp"
#include "view/View.hpp"

namespace ursql {

SQLInterpreter::SQLInterpreter(Interpreter* next) : Interpreter(next) {}

std::unique_ptr<Statement> SQLInterpreter::getStatement(Tokenizer& aTokenizer) {
    Keyword theKeyword = aTokenizer.peek().getKeyword();

    switch (theKeyword) {
    case Keyword::create_kw:
    case Keyword::describe_kw:
    case Keyword::drop_kw:
    case Keyword::desc_kw: {
        if (aTokenizer.remaining() > 1 &&
            aTokenizer.peek(1).getKeyword() == Keyword::table_kw)
        {
            return SQLStatement::factory(aTokenizer, *this);
        }
        break;
    }
    case Keyword::insert_kw:
    case Keyword::select_kw:
        return SQLStatement::factory(aTokenizer, *this);
    case Keyword::show_kw: {
        if (aTokenizer.remaining() > 1 &&
            aTokenizer.peek(1).getKeyword() == Keyword::tables_kw)
        {
            return SQLStatement::factory(aTokenizer, *this);
        }
        break;
    }
    case Keyword::delete_kw:
    case Keyword::update_kw:
        return SQLStatement::factory(aTokenizer, *this);
    default:
        return nullptr;
    }
    return nullptr;
}

StatusResult SQLInterpreter::createTable(
  const AttributeList& anAttributeList, const std::string& anEntityName) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        theResult = theActiveDB->createTable(anAttributeList, anEntityName);
        if (theResult) {
            theResult.setMessage("Query ok, table '" + anEntityName +
                                 "' created");
        }
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::describeTable(
  const std::string& anEntityName) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        Entity* theEntity =
          theActiveDB->getEntityByName(anEntityName, theResult);
        if (theResult) {
            if (theEntity) {
                DescTableView(*theEntity).show();
                theResult.setMessage(
                  std::to_string(theEntity->getAttributes().size()) +
                  " row(s) in set");
            } else {
                theResult.setError(Error::unknown_entity,
                                   "\'" + anEntityName + '\'');
            }
        }
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::dropTable(const std::string& anEntityName) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        size_type theRowCount;
        theResult = theActiveDB->dropTable(anEntityName, theRowCount);
        if (theResult) {
            theResult.setMessage("Query ok, " + std::to_string(theRowCount) +
                                 " row(s) affected");
        }
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::showTables() const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        StringList theEntityNames = theActiveDB->collectEntityNames();
        if (theEntityNames.empty()) {
            theResult.setMessage("empty set");
        } else {
            ShowTablesView(theActiveDB->getName(), theEntityNames).show();
            theResult.setMessage(std::to_string(theEntityNames.size()) +
                                 " row(s) in set");
        }
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::insertIntoTable(
  const std::string& anEntityName, const StringList& aFieldNames,
  const std::vector<StringList>& aValueStrsOfRows) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        theResult = theActiveDB->insertIntoTable(anEntityName, aFieldNames,
                                                 aValueStrsOfRows);
        if (theResult) {
            theResult.setMessage("Query ok, " +
                                 std::to_string(aValueStrsOfRows.size()) +
                                 " row(s) inserted");
        }
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::selectFromTable(const std::string& anEntityName,
                                             StringList& aFieldNames,
                                             const Filter* aFilter,
                                             const Order* anOrder) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        RowCollection theRowCollection;
        theResult = theActiveDB->selectFromTable(theRowCollection, anEntityName,
                                                 aFieldNames, aFilter, anOrder);
        if (theResult) {
            if (theRowCollection.empty()) {
                theResult.setMessage("empty set");
            } else {
                if (anOrder) {
                    theRowCollection.reorder(*anOrder);
                }
                SelectRowView(theRowCollection, aFieldNames).show();
                theResult.setMessage("Query ok, " +
                                     std::to_string(theRowCollection.size()) +
                                     " row(s) affected");
            }
        }
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::deleteFromTable(const std::string& anEntityName,
                                             const Filter* aFilter) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        theResult = theActiveDB->deleteFromTable(anEntityName, aFilter);
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

StatusResult SQLInterpreter::updateTable(const std::string& anEntityName,
                                         const Row::DataMap& aFieldValues,
                                         const Filter* aFilter) const {
    StatusResult theResult(Error::no_error);
    if (Database* theActiveDB = getActiveDatabase()) {
        theResult =
          theActiveDB->updateTable(anEntityName, aFieldValues, aFilter);
    } else {
        theResult.setError(Error::noDatabase_specified,
                           "Specify the database first by 'use <DBName>'");
    }
    return theResult;
}

}  // namespace ursql
