#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>

#include "Attribute.hpp"
#include "Entity.hpp"
#include "Row.hpp"
#include "Storage.hpp"
#include "TOC.hpp"

namespace UrSQL {

class Filter;
class Order;

class Database {
public:
    using EntityCache =
      std::unordered_map<std::string, std::unique_ptr<Entity> >;

    Database(const std::string& aFileName, CreateNewFile,
             StatusResult& aResult);

    Database(const std::string& aFileName, OpenExistingFile,
             StatusResult& aResult);

    ~Database();

    URSQL_DISABLE_COPY(Database);

    StatusResult createTable(const AttributeList& anAttributeList,
                             const std::string& anEntityName);

    StatusResult dropTable(const std::string& anEntityName,
                           size_type& aRowCount);

    StatusResult insertIntoTable(
      const std::string& anEntityName, const StringList& aFieldNames,
      const std::vector<StringList>& aValueStrsOfRows);

    StatusResult selectFromTable(RowCollection& aRowCollection,
                                 const std::string& anEntityName,
                                 StringList& aFieldNames, const Filter* aFilter,
                                 const Order* anOrder);

    StatusResult deleteFromTable(const std::string& anEntityName,
                                 const Filter* aFilter);

    StatusResult updateTable(const std::string& anEntityName,
                             const Row::DataMap& aFieldValues,
                             const Filter* aFilter);

    inline const std::string& getName() const {
        return m_storage.getName();
    }

    inline Storage& getStorage() {
        return m_storage;
    }

    Entity* getEntityByName(const std::string& anEntityName,
                            StatusResult& aResult);

    inline StringList collectEntityNames() const {
        return m_toc.collectEntityNames();
    }

private:
    TOC m_toc;
    Storage m_storage;
    EntityCache m_entityCache;

    inline bool _entityExists(const std::string& anEntityName) const {
        return m_toc.entityExists(anEntityName);
    }

    inline bool _entityCached(const std::string& anEntityName) const {
        return m_entityCache.count(anEntityName) == 1;
    }

    void _addEntityToCache(const std::string& anEntityName,
                           std::unique_ptr<Entity>&& anEntity);

    void _saveTOC();
    void _saveEntities();

    StatusResult _dropEntity(const std::string& anEntityName);
};

}  // namespace UrSQL
