#include "controller/DBManager.hpp"

#include "model/DBStatement.hpp"
#include "model/Database.hpp"
#include "model/FolderReader.hpp"
#include "model/Tokenizer.hpp"
#include "view/View.hpp"

namespace UrSQL {

DBManager::DBManager(Interpreter* anInterpreter)
    : Interpreter(anInterpreter),
      m_activeDB(nullptr) {}

std::unique_ptr<Statement> DBManager::getStatement(Tokenizer& aTokenizer) {
    Keyword theKeyword = aTokenizer.peek().getKeyword();

    switch (theKeyword) {
    case Keyword::create_kw:
    case Keyword::drop_kw:
    case Keyword::describe_kw:
    case Keyword::desc_kw: {
        if (aTokenizer.remaining() > 1 &&
            aTokenizer.peek(1).getKeyword() == Keyword::database_kw)
        {
            return DBStatement::factory(aTokenizer, *this);
        }
        break;
    }
    case Keyword::use_kw:
        return DBStatement::factory(aTokenizer, *this);
    case Keyword::show_kw: {
        if (aTokenizer.remaining() > 1 &&
            aTokenizer.peek(1).getKeyword() == Keyword::databases_kw)
        {
            return DBStatement::factory(aTokenizer, *this);
        }
        break;
    }
    default:
        return nullptr;
    }
    return nullptr;
}

StatusResult DBManager::createDatabase(const std::string& aName) {
    StatusResult theResult(Error::no_error);
    auto theNewDB =
      std::make_unique<Database>(aName, CreateNewFile{}, theResult);
    if (theResult) {
        _resetActiveDB(std::move(theNewDB));
        theResult.setMessage("Query OK, database '" + aName + "' created");
    }
    return theResult;
}

StatusResult DBManager::dropDatabase(const std::string& aName) {
    if (m_activeDB && m_activeDB->getName() == aName) {
        _releaseActiveDB();
    }
    StatusResult theResult = DBManager::_deleteDBFile(aName);
    if (theResult) {
        theResult.setMessage("Query OK, database '" + aName + "' dropped");
    }
    return theResult;
}

StatusResult DBManager::useDatabase(const std::string& aName) {
    StatusResult theResult(Error::no_error);
    if (!m_activeDB || m_activeDB->getName() != aName) {
        auto theNewDB =
          std::make_unique<Database>(aName, OpenExistingFile{}, theResult);
        if (theResult) {
            _resetActiveDB(std::move(theNewDB));
        }
    }
    if (theResult) {
        theResult.setMessage("Database changed");
    }
    return theResult;
}

StatusResult DBManager::showDatabases() {
    StringList theDBNames;
    StatusResult theResult = DBManager::_collectDBNames(theDBNames);
    if (theResult) {
        if (!theDBNames.empty()) {
            ShowDatabasesView(theDBNames).show();
            theResult.setMessage("Showing databases");
        }
        else {
            theResult.setMessage("empty set");
        }
    }
    return theResult;
}

StatusResult DBManager::describeDatabase(const std::string& aName) {
    Database* theDB = nullptr;
    StatusResult theResult(Error::no_error);
    std::unique_ptr<Database> theDBHolder(nullptr);
    if (m_activeDB && m_activeDB->getName() == aName) {
        theDB = m_activeDB.get();
    }
    else {
        theDBHolder =
          std::make_unique<Database>(aName, OpenExistingFile{}, theResult);
        theDB = theDBHolder.get();
    }
    if (theResult) {
        std::vector<BlockType> theBlockTypes;
        theResult = (theDB->getStorage())
                      .eachBlock([&theBlockTypes](Block& aBlock,
                                                  blocknum_t) -> StatusResult {
                          theBlockTypes.push_back(aBlock.getType());
                          return StatusResult(Error::no_error);
                      });
        if (theResult) {
            DescDatabaseView(theBlockTypes).show();
            theResult.setMessage("Describing database '" + aName + '\'');
        }
    }
    return theResult;
}

Database* DBManager::getActiveDatabase() const {
    return m_activeDB.get();
}

StatusResult DBManager::databaseExists(bool& exists, const std::string& aName) {
    std::string theFullName = aName + Storage::defaultFileExtension;
    FolderReader theFolderReader(Storage::defaultStoragePath);
    return theFolderReader.eachFile([&](const std::string& aFileName) {
        if (aFileName == theFullName) {
            exists = true;
        }
        return !exists;
    });
}

void DBManager::_resetActiveDB(std::unique_ptr<Database>&& aNewDB) {
    m_activeDB = std::move(aNewDB);
}

void DBManager::_releaseActiveDB() {
    _resetActiveDB(nullptr);
}

StatusResult DBManager::_deleteDBFile(const std::string& aName) {
    std::string theFilePath = Storage::getDBFilePath(aName);

    return std::remove(theFilePath.c_str()) == 0 ?
             StatusResult(Error::no_error) :
             StatusResult(Error::delete_error,
                          "Cannot delete file '" + theFilePath + '\'');
}

StatusResult DBManager::_collectDBNames(StringList& aDBNames) {
    return FolderReader(Storage::defaultStoragePath)
      .eachFile([&aDBNames](const std::string& aFileName) -> bool {
          if (Storage::hasDefaultExtension(aFileName)) {
              aDBNames.emplace_back(aFileName.substr(
                0, aFileName.length() - Storage::extensionLength));
          }
          return true;
      });
}

}  // namespace UrSQL
