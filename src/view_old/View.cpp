#include "view/View.hpp"

#include <algorithm>
#include <iomanip>

#include "model/Entity.hpp"
#include "model/FolderReader.hpp"
#include "model/Row.hpp"
#include "storage/Block.hpp"

namespace ursql {

/* -------------------------------ShowDatabasesView-------------------------------
 */
ShowDatabasesView::ShowDatabasesView(const StringList& aDBNames)
    : View(),
      m_dbNames(aDBNames) {}

void ShowDatabasesView::show() const {
    static const std::string theHeader = "Database";
    size_type theHorizontalWidth = theHeader.length();

    for (const auto& theDBName : m_dbNames) {
        theHorizontalWidth = std::max(theHorizontalWidth, theDBName.length());
    }

    const std::vector<size_type> theWidths({ theHorizontalWidth });

    View::printHorizontalLine(theWidths);
    View::printLine({ theHeader }, theWidths);
    View::printHorizontalLine(theWidths);

    for (const auto& theDBName : m_dbNames) {
        View::printLine({ theDBName }, theWidths);
    }

    View::printHorizontalLine(theWidths);
}

/* -------------------------------DescDatabaseView-------------------------------
 */
DescDatabaseView::DescDatabaseView(const std::vector<BlockType>& theTypes)
    : View(),
      m_types(theTypes) {}

void DescDatabaseView::show() const {
    static const std::string theFirstHeader = "Block", theSecondHeader = "Type";
    static const size_type theFirstWidth = theFirstHeader.length();
    static constexpr size_type theSecondWidth = 6;

    View::printHorizontalLine({ theFirstWidth, theSecondWidth });
    View::printLine({ theFirstHeader, theSecondHeader },
                    { theFirstWidth, theSecondWidth });
    View::printHorizontalLine({ theFirstWidth, theSecondWidth });

    for (size_type i = 0; i < m_types.size(); ++i) {
        BlockType theType = m_types[i];
        std::string theTypeString;
        switch (theType) {
        case BlockType::TOC_type:
            theTypeString = "TOC";
            break;
        case BlockType::entity_type:
            theTypeString = "Entity";
            break;
        case BlockType::index_type:
            theTypeString = "Index";
            break;
        case BlockType::row_type:
            theTypeString = "Row";
            break;
        case BlockType::free_type:
            theTypeString = "Free";
            break;
        }
        View::printLine({ std::to_string(i), theTypeString },
                        { theFirstWidth, theSecondWidth });
    }
    View::printHorizontalLine({ theFirstWidth, theSecondWidth });
}

/* -------------------------------DescTableView-------------------------------
 */
DescTableView::DescTableView(const Entity& anEntity)
    : View(),
      m_entity(anEntity) {}

void DescTableView::show() const {
    static const StringList theFieldTitles(
      { "Field", "Type", "Null", "Key", "Default", "Extra" });
    const auto& theAttributeList = m_entity.getAttributes();
    std::vector<StringList> theEntityDisplay;
    for (const auto& theAttribute : theAttributeList) {
        std::string theTypeString =
          View::valueType2String(theAttribute.getType());
        std::string theNullString = (theAttribute.isNullable() ? "YES" : "NO");
        std::string theKeyString = (theAttribute.isPrimary() ? "PRI" : "");
        std::string theDefaultValueString =
          theAttribute.getDefaultValue().toString();
        std::string theExtraString;
        theEntityDisplay.emplace_back(
          StringList{ theAttribute.getName(), theTypeString, theNullString,
                      theKeyString, theDefaultValueString, theExtraString });
    }
    std::vector<size_type> theWidths;
    for (const std::string& theFieldTitle : theFieldTitles) {
        theWidths.push_back(theFieldTitle.size());
    }
    for (const auto& theRowDisplay : theEntityDisplay) {
        for (size_type col = 0; col < theFieldTitles.size(); ++col) {
            theWidths[col] =
              std::max(theWidths[col], theRowDisplay[col].size());
        }
    }

    View::printHorizontalLine(theWidths);
    View::printLine(theFieldTitles, theWidths);
    View::printHorizontalLine(theWidths);
    for (const auto& theRowDisplay : theEntityDisplay) {
        View::printLine(theRowDisplay, theWidths);
    }
    View::printHorizontalLine(theWidths);
}

/* -------------------------------SelectRowView-------------------------------
 */
SelectRowView::SelectRowView(const RowCollection& aRowCollection,
                             const StringList& aFieldNames)
    : View(),
      m_rowCollection(aRowCollection),
      m_fieldNames(aFieldNames) {}

void SelectRowView::show() const {
    std::vector<StringList> theRowCollectionDisplay;
    m_rowCollection.eachRow([&](const Row& aRow) {
        StringList theRowDisplay;
        for (const std::string& theFieldName : m_fieldNames) {
            theRowDisplay.push_back(aRow.getField(theFieldName).toString());
        }
        theRowCollectionDisplay.push_back(std::move(theRowDisplay));
    });
    std::vector<size_type> theWidths;
    for (const std::string& theFieldName : m_fieldNames) {
        theWidths.push_back(theFieldName.size());
    }
    for (size_type i = 0; i < m_rowCollection.size(); ++i) {
        for (size_type j = 0; j < m_fieldNames.size(); ++j) {
            theWidths[j] =
              std::max(theWidths[j], theRowCollectionDisplay[i][j].size());
        }
    }
    View::printHorizontalLine(theWidths);
    View::printLine(m_fieldNames, theWidths);
    View::printHorizontalLine(theWidths);
    for (const StringList& theRowDisplay : theRowCollectionDisplay) {
        View::printLine(theRowDisplay, theWidths);
    }
    View::printHorizontalLine(theWidths);
}

/* -------------------------------ShowTablesView-------------------------------
 */
ShowTablesView::ShowTablesView(const std::string& aDBName,
                               const StringList& anEntityNames)
    : View(),
      m_dbName(aDBName),
      m_entityNames(anEntityNames) {}

void ShowTablesView::show() const {
    const std::string theTitle = "Tables_in_" + m_dbName;
    size_type theWidth = theTitle.size();
    for (const std::string& theEntityName : m_entityNames) {
        theWidth = std::max(theWidth, theEntityName.size());
    }
    const std::vector<size_type> theWidths({ theWidth });
    View::printHorizontalLine(theWidths);
    View::printLine({ theTitle }, theWidths);
    View::printHorizontalLine(theWidths);
    for (const std::string& theEntityName : m_entityNames) {
        View::printLine({ theEntityName }, theWidths);
    }
    View::printHorizontalLine(theWidths);
}

/* -------------------------------View Static
 * Methods------------------------------- */
void View::printHorizontalLine(const std::vector<size_type>& aWidths) {
    defaultOutput << View::vertex;
    for (size_type aWidth : aWidths) {
        std::fill_n(std::ostream_iterator<char>(defaultOutput), aWidth + 2,
                    horizontalEdge);
        defaultOutput << View::vertex;
    }
    defaultOutput << '\n';
}

void View::printLine(const StringList& aStrings,
                     const std::vector<size_type>& aWidths) {
    URSQL_TRUTH(aStrings.size() == aWidths.size(),
                "StringList size should be equal to aWidths size!");
    defaultOutput << View::verticalEdge;
    for (size_type i = 0; i < aStrings.size(); ++i) {
        defaultOutput << ' ' << std::setw(aWidths[i]) << std::left
                      << aStrings[i] << ' ' << View::verticalEdge;
    }
    defaultOutput << '\n';
}

std::string View::valueType2String(ValueType aType) {
    switch (aType) {
    case ValueType::int_type:
        return "int";
    case ValueType::float_type:
        return "float";
    case ValueType::bool_type:
        return "bool";
    case ValueType::varchar_type:
        return "varchar";
    default:
        URSQL_UNREACHABLE;
    }
}

}  // namespace ursql
