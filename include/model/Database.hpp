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
    using EntityCache =
      std::unordered_map<std::string, std::unique_ptr<Entity> >;

    Database(const std::string& fileName, CreateNewFile);

    Database(const std::string& fileName, OpenExistingFile);

    ~Database();

    URSQL_DISABLE_COPY(Database);

//    StatusResult createTable(const AttributeList& anAttributeList,
//                             const std::string& anEntityName);
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
//                                 StringList& aFieldNames, const Filter* aFilter,
//                                 const Order* anOrder);
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
//    Entity* getEntityByName(const std::string& anEntityName,
//                            StatusResult& aResult);
//
//    inline StringList collectEntityNames() const {
//        return m_toc.collectEntityNames();
//    }

private:
    TOC toc_;
    Storage storage_;
    EntityCache entityCache_;
};

}  // namespace ursql
