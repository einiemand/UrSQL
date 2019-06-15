#include "DBManager.hpp"
#include "Tokenizer.hpp"
#include "Database.hpp"
#include "FolderReader.hpp"
#include "DBStatement.hpp"
#include "View.hpp"

namespace UrSQL {

DBManager::DBManager(Interpreter* anInterpreter) :
	Interpreter(anInterpreter),
	m_activeDB(nullptr)
{
}

std::unique_ptr<Statement> DBManager::get_statement(Tokenizer& aTokenizer) {
	Keyword theKeyword = aTokenizer.peek().get_keyword();

	switch (theKeyword) {
	case Keyword::create_kw: {
		if (aTokenizer.remaining() > 1 && aTokenizer.peek(1).get_keyword() == Keyword::database_kw) {
			return DBStatement::factory(theKeyword, aTokenizer, *this);
		}
	}
	case Keyword::drop_kw: {
		if (aTokenizer.remaining() > 1 && aTokenizer.peek(1).get_keyword() == Keyword::database_kw) {
			return DBStatement::factory(theKeyword, aTokenizer, *this);
		}
	}
	case Keyword::use_kw:
		return DBStatement::factory(theKeyword, aTokenizer, *this);
	case Keyword::show_kw: {
		if (aTokenizer.remaining() > 1 && aTokenizer.peek(1).get_keyword() == Keyword::databases_kw) {
			return DBStatement::factory(theKeyword, aTokenizer, *this);
		}
	}
	case Keyword::describe_kw: {
		if (aTokenizer.remaining() > 1 && aTokenizer.peek(1).get_keyword() == Keyword::database_kw) {
			return DBStatement::factory(theKeyword, aTokenizer, *this);
		}
	}
	default:
		return nullptr;
	}
}

StatusResult DBManager::create_database(const std::string& aName) {
	StatusResult theResult(Error::no_error);
	auto theNewDB = std::make_unique<Database>(aName, CreateNewFile{}, theResult);
	if (theResult) {
		_reset_active_db(std::move(theNewDB));
		theResult.set_message("Query OK, database '" + aName + "' created");
	}
	return theResult;
}

StatusResult DBManager::drop_database(const std::string& aName) {
	if (m_activeDB && m_activeDB->get_name() == aName) {
		_release_active_db();
	}
	StatusResult theResult = DBManager::_delete_dbfile(aName);
	if (theResult) {
		theResult.set_message("Query OK, database '" + aName + "' dropped");
	}
	return theResult;
}

StatusResult DBManager::use_database(const std::string& aName) {
	StatusResult theResult(Error::no_error);
	if (!m_activeDB || m_activeDB->get_name() != aName) {
		auto theNewDB = std::make_unique<Database>(aName, OpenExistingFile{}, theResult);
		if (theResult) {
			_reset_active_db(std::move(theNewDB));
		}
	}
	if (theResult) {
		theResult.set_message("Database changed");
	}
	return theResult;
}

StatusResult DBManager::show_databases() {
	StringList theDBNames;
	StatusResult theResult = DBManager::_collect_dbnames(theDBNames);
	if (theResult) {
		if (!theDBNames.empty()) {
			ShowDBView(theDBNames).show();
			theResult.set_message("Showing databases");
		}
		else {
			theResult.set_message("empty set");
		}
	}
	return theResult;
}

StatusResult DBManager::describe_database(const std::string& aName) {
	Database* theDB = nullptr;
	StatusResult theResult(Error::no_error);
	std::unique_ptr<Database> theDBHolder(nullptr);
	if (m_activeDB && m_activeDB->get_name() == aName) {
		theDB = m_activeDB.get();
	}
	else {
		theDBHolder = std::make_unique<Database>(aName, OpenExistingFile{}, theResult);
		theDB = theDBHolder.get();
	}
	if (theResult) {
		DescDBView(theDB->get_storage()).show();
		theResult.set_message("Describing database '" + aName + '\'');
	}
	return theResult;
}

Database* DBManager::get_active_database() const {
	return m_activeDB.get();
}

StatusResult DBManager::database_exists(bool& exists, const std::string& aName) {
	std::string theFullName = aName + Storage::default_file_extension;
	FolderReader theFolderReader(Storage::default_storage_path);
	return theFolderReader.each_file(
		[&](const std::string& aFileName) {
			if (aFileName == theFullName) {
				exists = true;
			}
			return !exists;
		}
	);
}

void DBManager::_reset_active_db(std::unique_ptr<Database>&& aNewDB) {
	m_activeDB = std::move(aNewDB);
}

void DBManager::_release_active_db() {
	_reset_active_db(nullptr);
}

StatusResult DBManager::_delete_dbfile(const std::string& aName) {
	std::string theFilePath = Storage::get_dbfile_path(aName);

	return std::remove(theFilePath.c_str()) == 0 ? StatusResult(Error::no_error) :
		StatusResult(Error::delete_error, "Cannot delete file '" + theFilePath + '\'');
}

StatusResult DBManager::_collect_dbnames(StringList& aDBNames) {
	return FolderReader(Storage::default_storage_path).each_file(
		[&aDBNames](const std::string& aFileName)->bool {
			if (Storage::has_default_extension(aFileName)) {
				aDBNames.emplace_back(aFileName.substr(0, aFileName.length() - Storage::extension_length));
			}
			return true;
		}
	);
}

}