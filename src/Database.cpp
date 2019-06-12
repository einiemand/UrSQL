#include "Database.hpp"

namespace UrSQL {

Database::Database(const std::string& aFileName, CreateNewFile, StatusResult& aResult) :
	m_toc(),
	m_storage(aFileName, CreateNewFile{}, m_toc, aResult),
	m_tables()
{
}

Database::Database(const std::string& aFileName, OpenExistingFile, StatusResult& aResult) :
	m_toc(),
	m_storage(aFileName, OpenExistingFile{}, m_toc, aResult),
	m_tables()
{
}

} /* UrSQL */