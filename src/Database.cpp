#include "Database.hpp"

namespace UrSQL {

Database::Database(const std::string& aFileName, CreateNewFile, StatusResult& aResult) :
	m_toc(),
	m_storage(aFileName, CreateNewFile{}, m_toc, aResult),
	m_entityCache()
{
}

Database::Database(const std::string& aFileName, OpenExistingFile, StatusResult& aResult) :
	m_toc(),
	m_storage(aFileName, OpenExistingFile{}, m_toc, aResult),
	m_entityCache()
{
}

StatusResult Database::createTable(const std::vector<AttributeBuilder>& aBuilderList, const std::string& anEntityName) {
	StatusResult theResult(Error::no_error);
	if (!entityExists(anEntityName)) {
		auto theEntity = std::make_unique<Entity>();
		for (const auto& theBuilder : aBuilderList) {
			theEntity->addAttribute(theBuilder);
		}
		Block theBlock(*theEntity);
		blocknum_t theBlocknum = -1;
		theResult = m_storage.findFreeBlocknumber(theBlocknum);
		if (theResult) {
			theResult = m_storage.writeBlock(theBlock, theBlocknum);
			if (theResult) {
				m_toc.add(anEntityName, theBlocknum);
				_addEntityToCache(std::move(theEntity), anEntityName);
				theResult.setMessage("Query ok, table '" + anEntityName + "' created");
			}
		}
	}
	else {
		theResult.setError(Error::entity_exists, '\'' + anEntityName + '\'');
	}
	return theResult;
}

Entity* Database::getEntityByName(const std::string& anEntityName, StatusResult& aResult) {
	if (entityExists(anEntityName)) {
		if (!entityCached(anEntityName)) {
			blocknum_t theEntityPos = m_toc.getEntityPosByName(anEntityName);
			auto theEntity = std::make_unique<Entity>();
			aResult = m_storage.parseMonoStorable(*theEntity, theEntityPos);
			m_entityCache[anEntityName] = std::move(theEntity);
		}
		return m_entityCache.at(anEntityName).get();
	}
	throw std::runtime_error("Check if entity exists before getting it!");
}

} /* UrSQL */