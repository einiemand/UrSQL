#pragma once

#include "common/Error.hpp"

namespace UrSQL {

enum class ValueType : char;

class View {
public:
    View() = default;
    virtual ~View() = default;

    URSQL_DISABLE_COPY(View);

    virtual void show() const = 0;

protected:
    static constexpr char vertex = '+';
    static constexpr char verticalEdge = '|';
    static constexpr char horizontalEdge = '-';

    static void printHorizontalLine(const std::vector<size_type>& aWidths);

    static void printLine(const StringList& aFieldNames,
                          const std::vector<size_type>& aWidths);

    static std::string valueType2String(ValueType aType);
};

/* -------------------------------ShowDatabasesView-------------------------------
 */
class ShowDatabasesView : public View {
public:
    explicit ShowDatabasesView(const StringList& aDBNames);
    ~ShowDatabasesView() override = default;

    void show() const override;

private:
    const StringList& m_dbNames;
};

/* -------------------------------DescDatabaseView-------------------------------
 */
enum class BlockType : char;

class DescDatabaseView : public View {
public:
    explicit DescDatabaseView(const std::vector<BlockType>& theTypes);
    ~DescDatabaseView() override = default;

    void show() const override;

private:
    const std::vector<BlockType>& m_types;
};

/* -------------------------------DescTableView-------------------------------
 */
class Entity;

class DescTableView : public View {
public:
    explicit DescTableView(const Entity& anEntity);
    ~DescTableView() override = default;

    void show() const override;

private:
    const Entity& m_entity;
};

/* -------------------------------SelectRowView-------------------------------
 */
class RowCollection;

class SelectRowView : public View {
public:
    SelectRowView(const RowCollection& aRowCollection,
                  const StringList& aFieldNames);
    ~SelectRowView() override = default;

    void show() const override;

private:
    const RowCollection& m_rowCollection;
    const StringList& m_fieldNames;
};

/* -------------------------------ShowTablesView-------------------------------
 */
class ShowTablesView : public View {
public:
    ShowTablesView(const std::string& aDBName, const StringList& anEntityNames);
    ~ShowTablesView() override = default;

    void show() const override;

private:
    const std::string& m_dbName;
    const StringList& m_entityNames;
};

}  // namespace UrSQL
