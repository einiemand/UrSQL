#pragma once

#include <fstream>
#include <memory>
#include <unordered_map>

#include "model/Attribute.hpp"
#include "model/Entity.hpp"
#include "model/TOC.hpp"
#include "persistence/Storage.hpp"

namespace ursql {

class Filter;
class Order;

class Database {
public:
    using EntityCache = std::unordered_map<std::string, Entity>;

    Database(std::string name, const fs::path& filePath, CreateNewFile);
    Database(std::string name, const fs::path& filePath, OpenExistingFile);
    ~Database();

    URSQL_DISABLE_COPY(Database);

    [[nodiscard]] const std::string& getName() const;
    [[nodiscard]] std::vector<BlockType> getBlockTypes();
    [[nodiscard]] std::vector<std::string> getAllEntityNames() const;

    void createTable(const std::string& entityName,
                     const std::vector<Attribute>& attributes);

    void dropTables(const std::vector<std::string>& entityNames);

    //
    //    StatusResult dropTable(const std::string& anEntityName,
    //                           size_type& aRowCount);
    //
    //    StatusResult insertIntoTable(
    //      const std::string& anEntityName, const StringList& aFieldNames,
    //      const std::vector<StringList>& aValueStrsOfRows);
    //
    //    StatusResult selectFromTable(RowCollection& aRowCollection,
    //                                 const std::string& anEntityName,
    //                                 StringList& aFieldNames, const Filter*
    //                                 aFilter, const Order* anOrder);
    //
    //    StatusResult deleteFromTable(const std::string& anEntityName,
    //                                 const Filter* aFilter);
    //
    //    StatusResult updateTable(const std::string& anEntityName,
    //                             const Row::DataMap& aFieldValues,
    //                             const Filter* aFilter);
    //
    //    inline const std::string& getName() const {
    //        return m_storage.getName();
    //    }
    //
    //    inline Storage& getStorage() {
    //        return m_storage;
    //    }
    //
    //
    //    inline StringList collectEntityNames() const {
    //        return m_toc.collectEntityNames();
    //    }

private:
    std::string name_;
    Storage storage_;
    TOC toc_;
    EntityCache entityCache_;

    [[nodiscard]] std::size_t _findFreeBlockNumber();
    [[nodiscard]] Entity& _getEntityByName(const std::string& entityName);
    void _addEntity(const std::string& entityName, Entity& entity);
    void _dropEntity(const std::string& entityName);
};

}  // namespace ursql
