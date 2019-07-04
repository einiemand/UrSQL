#include "Database.hpp"
#include "View.hpp"
#include "Row.hpp"

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

Database::~Database() {
	_saveTOC();
	_saveEntites();
}

StatusResult Database::createTable(const AttributeList& anAttributeList, const std::string& anEntityName) {
	StatusResult theResult(Error::no_error);
	if (!_entityExists(anEntityName)) {
		blocknum_t theBlocknum;
		if (theResult = m_storage.findFreeBlocknumber(theBlocknum)) {
			auto theEntity = std::make_unique<Entity>(theBlocknum);
			for (const auto& theAttribute : anAttributeList) {
				theEntity->addAttribute(theAttribute);
			}
			if (theResult) {
				theResult = m_storage.writeBlock(Block(*theEntity), theBlocknum);
				if (theResult) {
					m_toc.add(anEntityName, theBlocknum);
					_addEntityToCache(std::move(theEntity), anEntityName);
				}
			}
		}
	}
	else {
		theResult.setError(Error::entity_exists, '\'' + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::describeTable(const std::string& anEntityName, size_type& theAttributeCount) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		if (theResult) {
			DescTableView(*theEntity).show();
			theAttributeCount = theEntity->getAttributes().size();
		}
	}
	else {
		theResult.setError(Error::unknown_entity, "\'" + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::insertIntoTable(const std::string& anEntityName, const StringList& aFieldNames, const StringList& aValueStrs) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		blocknum_t theBlocknum;
		if (theResult = m_storage.findFreeBlocknumber(theBlocknum)) {
			Entity* theEntity = getEntityByName(anEntityName, theResult);
			Row theNewRow(theBlocknum);
			if (theResult = theEntity->generateNewRow(theNewRow, aFieldNames, aValueStrs)) {
				theResult = m_storage.writeBlock(Block(*theEntity), theBlocknum);
			}
		}
	}
	else {
		theResult.setError(Error::unknown_entity, '\'' + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::selectFromTable(RowCollection& aRowCollection, const std::string& anEntityName, StringList& aFieldNames) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		for (auto iter = aFieldNames.cbegin(); iter != aFieldNames.cend() && theResult; ++iter) {
			const std::string& theFieldName = *iter;
			if (!theEntity->attributeExistsByName(theFieldName)) {
				theResult.setError(Error::unknown_attribute, '\'' + theFieldName + '\'');
			}
		}
		if (theResult) {
			theResult = m_storage.visitBlocks(
				[&aRowCollection](Block& aBlock, blocknum_t aBlocknum)->StatusResult {
					Row theRow(aBlocknum);
					theRow.decode(aBlock);
					aRowCollection.addRow(std::move(theRow));
					return StatusResult(Error::no_error);
				},
				theEntity->getRowPos());
			if (theResult && aFieldNames.empty()) {
				for (const Attribute& theAttribute : theEntity->getAttributes()) {
					aFieldNames.push_back(theAttribute.getName());
				}
			}
		}
	}
	else {
		theResult.setError(Error::unknown_entity, '\'' + anEntityName + '\'');
	}
	return theResult;
}

Entity* Database::getEntityByName(const std::string& anEntityName, StatusResult& aResult) {
	if (_entityExists(anEntityName)) {
		if (!_entityCached(anEntityName)) {
			blocknum_t theEntityPos = m_toc.getEntityPosByName(anEntityName);
			auto theEntity = std::make_unique<Entity>(theEntityPos);
			aResult = m_storage.decodeMonoStorable(*theEntity);
			if (aResult) {
				m_entityCache[anEntityName] = std::move(theEntity);
			}
		}
		return m_entityCache.at(anEntityName).get();
	}
	throw std::runtime_error("Check if entity exists before getting it!");
}

void Database::_addEntityToCache(std::unique_ptr<Entity>&& anEntity, const std::string& anEntityName) {
	if (_entityCached(anEntityName)) {
		throw std::runtime_error(anEntityName + " already cached");
	}
	m_entityCache.insert({ anEntityName,std::move(anEntity) });
}

void Database::_saveTOC() {
	if (m_toc.isDirty()) {
		m_storage.writeBlock(Block(m_toc), 0);
	}
}

void Database::_saveEntites() {
	for (const auto& theNameAndEntity : m_entityCache) {
		Entity* theEntity = theNameAndEntity.second.get();
		if (theEntity->isDirty()) {
			blocknum_t theBlocknum = theEntity->getBlocknum();
			m_storage.writeBlock(Block(*theEntity), theBlocknum);
		}
	}
}

} /* UrSQL */