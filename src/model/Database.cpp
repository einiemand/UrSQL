#include "model/Database.hpp"

#include <format>
#include <numeric>

#include "exception/UserError.hpp"
#include "model/Entity.hpp"
#include "model/Row.hpp"

namespace ursql {

namespace {

std::vector<bool> validateSpecifiedAttributes(
  const std::vector<Attribute>& attributes,
  const std::vector<std::size_t>& attrIndexes) {
    std::vector<bool> attrSpecified(attributes.size(), false);
    for (std::size_t index : attrIndexes) {
        URSQL_EXPECT(!attrSpecified[index], InvalidCommand,
                     std::format("'{}' specified more than once",
                                 attributes[index].getName()));
        attrSpecified[index] = true;
    }
    return attrSpecified;
}

void validateInsertValueLists(const std::vector<std::vector<Value>>& valueLists,
                              const std::vector<Attribute>& attributes,
                              const std::vector<std::size_t>& attrIndexes) {
    for (auto& valueList : valueLists) {
        URSQL_EXPECT(valueList.size() == attrIndexes.size(), MisMatch,
                     "column count and value count");
        for (std::size_t i = 0; i < valueList.size(); ++i) {
            if (valueList[i].isNull()) {
                auto& attribute = attributes[attrIndexes[i]];
                URSQL_EXPECT(
                  attribute.isNullable(), InvalidCommand,
                  std::format("'{}' can't be null", attribute.getName()));
            }
        }
    }
}

}  // namespace

Database::Database(std::string name, const fs::path& filePath, CreateNewFile)
    : name_(std::move(name)),
      storage_(filePath, CreateNewFile{}),
      toc_(),
      entityCache_() {
    storage_.save(toc_);
}

Database::Database(std::string name, const fs::path& filePath, OpenExistingFile)
    : name_(std::move(name)),
      storage_(filePath, OpenExistingFile{}),
      toc_(),
      entityCache_() {
    storage_.load(toc_);
}

Database::~Database() {
    storage_.save(toc_);
    for (auto& [_, entity] : entityCache_) {
        storage_.save(entity);
    }
}

const std::string& Database::getName() const {
    return name_;
}

std::vector<BlockType> Database::getBlockTypes() {
    std::size_t blockCnt = storage_.getBlockCount();
    std::vector<BlockType> blockTypes;
    blockTypes.reserve(blockCnt);
    for (std::size_t i = 0; i < blockCnt; ++i) {
        blockTypes.push_back(storage_.getBlockType(i));
    }
    return blockTypes;
}

std::vector<std::string> Database::getAllEntityNames() const {
    return toc_.getAllEntityNames();
}

void Database::createTable(const std::string& entityName,
                           const std::vector<Attribute>& attributes) {
    URSQL_EXPECT(!toc_.entityExists(entityName), AlreadyExists, entityName);
    std::size_t blockNum = _findFreeBlockNumber();
    Entity entity(blockNum);
    entity.setAttributes(attributes);
    _addEntity(entityName, entity);
}

void Database::dropTables(const std::vector<std::string>& entityNames) {
    for (auto& entityName : entityNames) {
        URSQL_EXPECT(toc_.entityExists(entityName), DoesNotExist, entityName);
    }
    for (auto& entityName : entityNames) {
        _dropEntity(entityName);
    }
}

void Database::insertIntoTable(
  const std::string& entityName,
  const std::optional<std::vector<std::string>>& attrNamesOpt,
  const std::vector<std::vector<Value>>& valueLists) {
    Entity& entity = _getEntityByName(entityName);
    std::vector<std::size_t> attrIndexes;
    if (attrNamesOpt.has_value()) {
        auto& attrNames = attrNamesOpt.value();
        attrIndexes.reserve(attrNames.size());
        for (auto& attrName : attrNames) {
            attrIndexes.push_back(entity.attributeIndex(attrName));
        }
    } else {
        auto& attributes = entity.getAttributes();
        attrIndexes.resize(attributes.size());
        std::iota(std::begin(attrIndexes), std::end(attrIndexes), 0);
    }
    _insertIntoTableInternal(entity, attrIndexes, valueLists);
}

std::size_t Database::_findFreeBlockNumber() {
    std::size_t blockCnt = storage_.getBlockCount();
    for (std::size_t i = 0; i < blockCnt; ++i) {
        if (storage_.getBlockType(i) == BlockType::free) {
            return i;
        }
    }
    return blockCnt;
}

// StatusResult Database::dropTable(const std::string& anEntityName,
//                                  size_type& aRowCount) {
//     StatusResult theResult(Error::no_error);
//     if (_entityExists(anEntityName)) {
//         Entity* theEntity = getEntityByName(anEntityName, theResult);
//         const auto& theRowPositions = theEntity->getRowPos();
//         aRowCount = theRowPositions.size();
//         if (theResult) {
//             theResult = m_storage.releaseBlocks(theRowPositions);
//             if (theResult) {
//                 theResult = _dropEntity(anEntityName);
//             }
//         }
//     } else {
//         theResult.setError(Error::unknown_entity, '\'' + anEntityName +
//         '\'');
//     }
//     return theResult;
// }
//
// StatusResult Database::insertIntoTable(
//   const std::string& anEntityName, const StringList& aFieldNames,
//   const std::vector<StringList>& aValueStrsOfRows) {
//     StatusResult theResult(Error::no_error);
//     if (_entityExists(anEntityName)) {
//         Entity* theEntity = getEntityByName(anEntityName, theResult);
//         for (auto iter = aValueStrsOfRows.cbegin();
//              theResult && iter != aValueStrsOfRows.cend(); ++iter)
//         {
//             const StringList& theRowValueStrs = *iter;
//             blocknum_t theBlocknum;
//             theResult = m_storage.findFreeBlocknumber(theBlocknum);
//             if (theResult) {
//                 Row theNewRow(theBlocknum);
//                 theResult = theEntity->generateNewRow(theNewRow, aFieldNames,
//                                                       theRowValueStrs);
//                 if (theResult) {
//                     theResult = m_storage.saveMonoStorable(theNewRow);
//                     if (theResult) {
//                         theEntity->addRowPosition(theBlocknum);
//                     }
//                 }
//             }
//         }
//     } else {
//         theResult.setError(Error::unknown_entity, '\'' + anEntityName +
//         '\'');
//     }
//     return theResult;
// }
//
// StatusResult Database::selectFromTable(RowCollection& aRowCollection,
//                                        const std::string& anEntityName,
//                                        StringList& aFieldNames,
//                                        const Filter* aFilter,
//                                        const Order* anOrder) {
//     StatusResult theResult(Error::no_error);
//     if (_entityExists(anEntityName)) {
//         Entity* theEntity = getEntityByName(anEntityName, theResult);
//         for (auto iter = aFieldNames.cbegin();
//              iter != aFieldNames.cend() && theResult; ++iter)
//         {
//             const std::string& theFieldName = *iter;
//             if (!theEntity->attributeExistsByName(theFieldName)) {
//                 theResult.setError(Error::unknown_attribute,
//                                    '\'' + theFieldName + '\'');
//             }
//         }
//         if (theResult && aFilter) {
//             theResult = aFilter->validate(*theEntity);
//         }
//         if (theResult && anOrder) {
//             theResult = anOrder->validate(*theEntity);
//         }
//         if (theResult) {
//             theResult = m_storage.visitBlocks(
//               [&aRowCollection, aFilter](Block& aBlock,
//                                          blocknum_t aBlocknum) ->
//                                          StatusResult {
//                   Row theRow(aBlocknum);
//                   theRow.decode(aBlock);
//                   if (!aFilter || aFilter->match(theRow)) {
//                       aRowCollection.addRow(std::move(theRow));
//                   }
//                   return StatusResult(Error::no_error);
//               },
//               theEntity->getRowPos());
//             if (theResult && aFieldNames.empty()) {
//                 for (const Attribute& theAttribute :
//                 theEntity->getAttributes())
//                 {
//                     aFieldNames.push_back(theAttribute.getName());
//                 }
//             }
//         }
//     } else {
//         theResult.setError(Error::unknown_entity, '\'' + anEntityName +
//         '\'');
//     }
//     return theResult;
// }
//
// StatusResult Database::deleteFromTable(const std::string& anEntityName,
//                                        const Filter* aFilter) {
//     StatusResult theResult(Error::no_error);
//     if (_entityExists(anEntityName)) {
//         Entity* theEntity = getEntityByName(anEntityName, theResult);
//         if (theResult && aFilter) {
//             theResult = aFilter->validate(*theEntity);
//         }
//         if (theResult) {
//             std::vector<blocknum_t> theBlocknums;
//             theResult = m_storage.visitBlocks(
//               [aFilter, &theBlocknums](Block& aBlock, blocknum_t aBlocknum) {
//                   if (aFilter) {
//                       Row theRow(aBlocknum);
//                       theRow.decode(aBlock);
//                       if (aFilter->match(theRow)) {
//                           theBlocknums.push_back(aBlocknum);
//                       }
//                   }
//                   return StatusResult(Error::no_error);
//               },
//               theEntity->getRowPos());
//
//             if (theResult) {
//                 theResult = m_storage.releaseBlocks(theBlocknums);
//                 if (theResult) {
//                     for (blocknum_t theBlocknum : theBlocknums) {
//                         theEntity->dropRowPosition(theBlocknum);
//                     }
//                     theResult.setMessage("Query ok, " +
//                                          std::to_string(theBlocknums.size())
//                                          + " row(s) affected");
//                 }
//             }
//         }
//     } else {
//         theResult.setError(Error::unknown_entity, '\'' + anEntityName +
//         '\'');
//     }
//     return theResult;
// }
//
// StatusResult Database::updateTable(const std::string& anEntityName,
//                                    const Row::DataMap& aFieldValues,
//                                    const Filter* aFilter) {
//     StatusResult theResult(Error::no_error);
//     if (_entityExists(anEntityName)) {
//         Entity* theEntity = getEntityByName(anEntityName, theResult);
//         for (auto iter = aFieldValues.cbegin();
//              theResult && iter != aFieldValues.cend(); ++iter)
//         {
//             const std::string& theAttributeName = iter->first;
//             if (theEntity->attributeExistsByName(theAttributeName)) {
//                 const Attribute& theAttribute =
//                   theEntity->getAttributeByName(theAttributeName);
//                 if (theAttribute.getType() != iter->second.type()) {
//                     theResult.setError(Error::keyValue_mismatch,
//                                        '\'' + theAttributeName + '\'');
//                 }
//             } else {
//                 theResult.setError(Error::unknown_attribute,
//                                    '\'' + theAttributeName + '\'');
//             }
//         }
//
//         if (theResult && aFilter) {
//             theResult = aFilter->validate(*theEntity);
//         }
//
//         if (theResult) {
//             size_type theRowCount = 0;
//             theResult = m_storage.visitBlocks(
//               [this, aFilter, &aFieldValues, &theRowCount](
//                 Block& aBlock, blocknum_t aBlocknum) {
//                   StatusResult theSubResult(Error::no_error);
//                   Row theRow(aBlocknum);
//                   theRow.decode(aBlock);
//                   if (!aFilter || aFilter->match(theRow)) {
//                       for (const auto& theKVPair : aFieldValues) {
//                           theRow.updateField(theKVPair.first,
//                           theKVPair.second);
//                       }
//                       theSubResult = m_storage.saveMonoStorable(theRow);
//                       if (theSubResult) {
//                           ++theRowCount;
//                       }
//                   }
//                   return theSubResult;
//               },
//               theEntity->getRowPos());
//
//             if (theResult) {
//                 theResult.setMessage("Query ok, " +
//                                      std::to_string(theRowCount) +
//                                      " row(s) affected");
//             }
//         }
//         return theResult;
//     } else {
//         theResult.setError(Error::unknown_entity, '\'' + anEntityName +
//         '\'');
//     }
//     return theResult;
// }
//
Entity& Database::_getEntityByName(const std::string& entityName) {
    auto it = entityCache_.find(entityName);
    if (it == std::end(entityCache_)) {
        std::size_t blockNum = toc_.getEntityPosByName(entityName);
        Entity entity(blockNum);
        storage_.load(entity);
        it = entityCache_.emplace(entityName, std::move(entity)).first;
    }
    return it->second;
}

void Database::_addEntity(const std::string& entityName, Entity& entity) {
    storage_.save(entity);
    toc_.addEntity(entityName, entity.getBlockNum());
    entityCache_.emplace(entityName, std::move(entity));
}

void Database::_dropEntity(const std::string& entityName) {
    Entity& entity = _getEntityByName(entityName);
    for (std::size_t rowBlockNum : entity.getRowBlockNums()) {
        storage_.releaseBlock(rowBlockNum);
    }
    storage_.releaseBlock(entity.getBlockNum());
    toc_.dropEntity(entityName);
    entityCache_.erase(entityName);
}

void Database::_insertIntoTableInternal(
  Entity& entity, const std::vector<std::size_t>& attrIndexes,
  const std::vector<std::vector<Value>>& valueLists) {
    auto& attributes = entity.getAttributes();
    std::vector<bool> attrSpecified =
      validateSpecifiedAttributes(attributes, attrIndexes);
    validateInsertValueLists(valueLists, attributes, attrIndexes);
    std::vector<std::vector<Value>> valueRows;
    valueRows.reserve(valueLists.size());
    for (auto& valueList : valueLists) {
        std::vector<Value> valueRow(attributes.size());
        for (std::size_t i = 0; i < attributes.size(); ++i) {
            if (!attrSpecified[i]) {
                auto& attribute = attributes[i];
                URSQL_EXPECT(
                  !attribute.mustBeSpecified(), InvalidCommand,
                  std::format("'{}' must be specified", attribute.getName()));
                valueRow[i] =
                  attribute.isAutoInc() ?
                    Value(static_cast<Value::int_t>(entity.getNextAutoInc())) :
                    attribute.getDefaultValue();
            }
        }
        for (std::size_t i = 0; i < attrIndexes.size(); ++i) {
            std::size_t attrIndex = attrIndexes[i];
            auto& attribute = attributes[attrIndex];
            valueRow[attrIndex] = valueList[i].cast(attribute.getType());
            if (attribute.isAutoInc()) {
                entity.updateAutoInc(
                  valueRow[attrIndex].raw<ValueType::int_type>());
            }
        }
        valueRows.push_back(std::move(valueRow));
    }
    for (auto& valueRow : valueRows) {
        std::size_t blockNum = _findFreeBlockNumber();
        Row row(blockNum, std::move(valueRow));
        storage_.save(row);
        entity.addRowPosition(blockNum);
    }
}

//
// void Database::_addEntityToCache(const std::string& anEntityName,
//                                  std::unique_ptr<Entity>&& anEntity) {
//     URSQL_TRUTH(!_entityCached(anEntityName),
//                 '\'' + anEntityName + "' already cached");
//     m_entityCache.insert({ anEntityName, std::move(anEntity) });
// }
//
// void Database::_saveTOC() {
//     if (m_toc.isDirty()) {
//         m_storage.saveMonoStorable(m_toc);
//     }
// }
//
// void Database::_saveEntities() {
//     for (const auto& theNameAndEntity : m_entityCache) {
//         Entity* theEntity = theNameAndEntity.second.get();
//         if (theEntity->isDirty()) {
//             m_storage.saveMonoStorable(*theEntity);
//         }
//     }
// }
//
// StatusResult Database::_dropEntity(const std::string& anEntityName) {
//     URSQL_TRUTH(_entityExists(anEntityName),
//                 "Trying to drop an entity that doesn't exist!");
//     blocknum_t theBlocknum = m_toc.getEntityPosByName(anEntityName);
//     StatusResult theResult = m_storage.releaseBlock(theBlocknum);
//     if (theResult) {
//         if (_entityCached(anEntityName)) {
//             m_entityCache.erase(anEntityName);
//         }
//         m_toc.drop(anEntityName);
//     }
//     return theResult;
// }

}  // namespace ursql
