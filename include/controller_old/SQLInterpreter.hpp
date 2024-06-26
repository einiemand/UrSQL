#pragma once

#include "Interpreter.hpp"
#include "model/Attribute.hpp"
#include "model/Row.hpp"

namespace ursql {

class Filter;
class Order;

class SQLInterpreter : public Interpreter {
public:
    explicit SQLInterpreter(Interpreter* next);
    ~SQLInterpreter() override = default;

    std::unique_ptr<Statement> getStatement(Tokenizer& aTokenizer) override;

    StatusResult createTable(const AttributeList& anAttributeList,
                             const std::string& anEntityName) const;

    StatusResult describeTable(const std::string& anEntityName) const;
    StatusResult dropTable(const std::string& anEntityName) const;
    StatusResult showTables() const;

    StatusResult insertIntoTable(
      const std::string& anEntityName, const StringList& aFieldNames,
      const std::vector<StringList>& aValueStrsOfRows) const;

    StatusResult selectFromTable(const std::string& anEntityName,
                                 StringList& aFieldNames, const Filter* aFilter,
                                 const Order* anOrder) const;

    StatusResult deleteFromTable(const std::string& anEntityName,
                                 const Filter* aFilter) const;

    StatusResult updateTable(const std::string& anEntityName,
                             const Row::DataMap& aFieldValues,
                             const Filter* aFilter) const;
};

}  // namespace ursql
