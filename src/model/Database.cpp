#include "model/Database.hpp"
#include "model/Filter.hpp"
#include "model/Order.hpp"

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
	_saveEntities();
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
				theResult = m_storage.saveMonoStorable(*theEntity);
				if (theResult) {
					m_toc.add(anEntityName, theBlocknum);
					_addEntityToCache(anEntityName, std::move(theEntity));
				}
			}
		}
	}
	else {
		theResult.setError(Error::entity_exists, '\'' + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::dropTable(const std::string& anEntityName, size_type& aRowCount) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		const auto& theRowPositions = theEntity->getRowPos();
		aRowCount = theRowPositions.size();
		if (theResult) {
			theResult = m_storage.releaseBlocks(theRowPositions);
			if (theResult) {
				theResult = _dropEntity(anEntityName);
			}
		}
	}
	else {
		theResult.setError(Error::unknown_entity, '\'' + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::insertIntoTable(const std::string& anEntityName, const StringList& aFieldNames, const std::vector<StringList>& aValueStrsOfRows) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		for (auto iter = aValueStrsOfRows.cbegin(); theResult && iter != aValueStrsOfRows.cend(); ++iter) {
			const StringList& theRowValueStrs = *iter;
			blocknum_t theBlocknum;
			if (theResult = m_storage.findFreeBlocknumber(theBlocknum)) {
				Row theNewRow(theBlocknum);
				if (theResult = theEntity->generateNewRow(theNewRow, aFieldNames, theRowValueStrs)) {
					theResult = m_storage.saveMonoStorable(theNewRow);
					if (theResult) {
						theEntity->addRowPosition(theBlocknum);
					}
				}
			}
		}
	}
	else {
		theResult.setError(Error::unknown_entity, '\'' + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::selectFromTable(
	RowCollection& aRowCollection,
	const std::string& anEntityName,
	StringList& aFieldNames,
	const Filter* aFilter,
	const Order* anOrder) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		for (auto iter = aFieldNames.cbegin(); iter != aFieldNames.cend() && theResult; ++iter) {
			const std::string& theFieldName = *iter;
			if (!theEntity->attributeExistsByName(theFieldName)) {
				theResult.setError(Error::unknown_attribute, '\'' + theFieldName + '\'');
			}
		}
		if (theResult && aFilter) {
			theResult = aFilter->validate(*theEntity);
		}
		if (theResult && anOrder) {
			theResult = anOrder->validate(*theEntity);
		}
		if (theResult) {
			theResult = m_storage.visitBlocks(
				[&aRowCollection, aFilter](Block& aBlock, blocknum_t aBlocknum)->StatusResult {
					auto theRow = std::make_unique<Row>(aBlocknum);
					theRow->decode(aBlock);
					if (!aFilter || aFilter->match(*theRow)) {
						aRowCollection.addRow(std::move(theRow));
					}
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

StatusResult Database::deleteFromTable(const std::string& anEntityName, const Filter* aFilter) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		if (theResult && aFilter) {
			theResult = aFilter->validate(*theEntity);
		}
		if (theResult) {
			std::vector<blocknum_t> theBlocknums;
			theResult = m_storage.visitBlocks(
				[aFilter, &theBlocknums](Block& aBlock, blocknum_t aBlocknum) {
					if (aFilter) {
						Row theRow(aBlocknum);
						theRow.decode(aBlock);
						if (aFilter->match(theRow)) {
							theBlocknums.push_back(aBlocknum);
						}
					}
					return StatusResult(Error::no_error);
				},
				theEntity->getRowPos());

			if (theResult) {
				theResult = m_storage.releaseBlocks(theBlocknums);
				if (theResult) {
					for (blocknum_t theBlocknum : theBlocknums) {
						theEntity->dropRowPosition(theBlocknum);
					}
					theResult.setMessage("Query ok, " + std::to_string(theBlocknums.size()) + " row(s) affected");
				}
			}
		}
	}
	else {
		theResult.setError(Error::unknown_entity, '\'' + anEntityName + '\'');
	}
	return theResult;
}

StatusResult Database::updateTable(const std::string& anEntityName, const Row::DataMap& aFieldValues, const Filter* aFilter) {
	StatusResult theResult(Error::no_error);
	if (_entityExists(anEntityName)) {
		Entity* theEntity = getEntityByName(anEntityName, theResult);
		for (auto iter = aFieldValues.cbegin(); theResult && iter != aFieldValues.cend(); ++iter) {
			const std::string& theAttributeName = iter->first;
			if (theEntity->attributeExistsByName(theAttributeName)) {
				const Attribute& theAttribute = theEntity->getAttributeByName(theAttributeName);
				if (theAttribute.getType() != iter->second.getType()) {
					theResult.setError(Error::keyValue_mismatch, '\'' + theAttributeName + '\'');
				}
			}
			else {
				theResult.setError(Error::unknown_attribute, '\'' + theAttributeName + '\'');
			}
		}

		if (theResult && aFilter) {
			theResult = aFilter->validate(*theEntity);
		}

		if (theResult) {
			size_type theRowCount = 0;
			theResult = m_storage.visitBlocks(
				[this, aFilter, &aFieldValues, &theRowCount](Block& aBlock, blocknum_t aBlocknum) {
					StatusResult theSubResult(Error::no_error);
					Row theRow(aBlocknum);
					theRow.decode(aBlock);
					if (!aFilter || aFilter->match(theRow)) {
						for (const auto& theKVPair : aFieldValues) {
							theRow.updateField(theKVPair.first, theKVPair.second);
						}
						theSubResult = m_storage.saveMonoStorable(theRow);
						if (theSubResult) {
							++theRowCount;
						}
					}
					return theSubResult;
				},
				theEntity->getRowPos());

			if (theResult) {
				theResult.setMessage("Query ok, " + std::to_string(theRowCount) + " row(s) affected");
			}
		}
		return theResult;
	}
	else {
		theResult.setError(Error::unknown_entity, '\'' + anEntityName + '\'');
	}
	return theResult;
}

Entity* Database::getEntityByName(const std::string& anEntityName, StatusResult& aResult) {
	Entity* theObserver = nullptr;
	if (_entityExists(anEntityName)) {
		if (!_entityCached(anEntityName)) {
			blocknum_t theEntityPos = m_toc.getEntityPosByName(anEntityName);
			auto theEntity = std::make_unique<Entity>(theEntityPos);
			aResult = m_storage.decodeMonoStorable(*theEntity);
			if (aResult) {
				_addEntityToCache(anEntityName, std::move(theEntity));
			}
		}
		if (aResult) {
			theObserver = m_entityCache[anEntityName].get();
		}
	}
	return theObserver;
}

void Database::_addEntityToCache(const std::string& anEntityName, std::unique_ptr<Entity>&& anEntity) {
	URSQL_TRUTH(!_entityCached(anEntityName), '\'' + anEntityName + "' already cached");
	m_entityCache.insert({ anEntityName,std::move(anEntity) });
}

void Database::_saveTOC() {
	if (m_toc.isDirty()) {
		m_storage.saveMonoStorable(m_toc);
	}
}

void Database::_saveEntities() {
	for (const auto& theNameAndEntity : m_entityCache) {
		Entity* theEntity = theNameAndEntity.second.get();
		if (theEntity->isDirty()) {
			m_storage.saveMonoStorable(*theEntity);
		}
	}
}

StatusResult Database::_dropEntity(const std::string& anEntityName) {
	URSQL_TRUTH(_entityExists(anEntityName), "Trying to drop an entity that doesn't exist!");
	blocknum_t theBlocknum = m_toc.getEntityPosByName(anEntityName);
	StatusResult theResult = m_storage.releaseBlock(theBlocknum);
	if (theResult) {
		if (_entityCached(anEntityName)) {
			m_entityCache.erase(anEntityName);
		}
		m_toc.drop(anEntityName);
	}
	return theResult;
}

} /* UrSQL */