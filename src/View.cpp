#include "View.hpp"
#include "FolderReader.hpp"
#include "Block.hpp"
#include <algorithm>
#include <iomanip>

namespace UrSQL {

/* -------------------------------ShowDatabasesView------------------------------- */
ShowDatabasesView::ShowDatabasesView(const StringList& aDBNames) :
	View(),
	m_dbNames(aDBNames)
{
}

void ShowDatabasesView::show() const {
	static const std::string theHeader = "Database";
	size_type theHorizontalWidth = theHeader.length();

	for (const auto& theDBName : m_dbNames) {
		theHorizontalWidth = std::max(theHorizontalWidth, theDBName.length());
	}
	theHorizontalWidth += 2;

	const std::vector<size_type> theWidths({ theHorizontalWidth });

	View::printHorizontalLine(theWidths);
	View::printLine({ theHeader }, theWidths);
	View::printHorizontalLine(theWidths);

	for (const auto& theDBName : m_dbNames) {
		View::printLine({ theDBName }, theWidths);
	}

	View::printHorizontalLine(theWidths);
}

/* -------------------------------DescDatabaseView------------------------------- */
DescDatabaseView::DescDatabaseView(const std::vector<BlockType>& theTypes) :
	View(),
	m_types(theTypes)
{
}

void DescDatabaseView::show() const {
	static const std::string theFirstHeader = "Block", theSecondHeader = "Type";
	static const size_type theFirstWidth = theFirstHeader.length() + 2;
	static constexpr size_type theSecondWidth = 6 + 2;

	View::printHorizontalLine({ theFirstWidth,theSecondWidth });
	View::printLine({ theFirstHeader,theSecondHeader }, { theFirstWidth,theSecondWidth });
	View::printHorizontalLine({ theFirstWidth,theSecondWidth });

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
		View::printLine({ std::to_string(i),theTypeString }, { theFirstWidth,theSecondWidth });
	}
	View::printHorizontalLine({ theFirstWidth,theSecondWidth });
}

/* -------------------------------DescTableView------------------------------- */
DescTableView::DescTableView(const Entity& anEntity) :
	View(),
	m_entity(anEntity)
{
}

void DescTableView::show() const {

}

/* -------------------------------View Static Methods------------------------------- */
void View::printHorizontalLine(const std::vector<size_type>& aWidths) {
	defaultOutput << View::vertex;
	for (size_type aWidth : aWidths) {
		defaultOutput << std::string(aWidth, View::horizontalEdge) << View::vertex;
	}
	defaultOutput << '\n';
}

void View::printLine(const StringList& aStrings, const std::vector<size_type>& aWidths) {
	if (aStrings.size() != aWidths.size()) {
		throw std::runtime_error("StringList size should be equal to aWidths size!");
	}
	size_type i = 0;
	defaultOutput << View::verticalEdge;
	for (; i < aStrings.size(); ++i) {
		defaultOutput << std::setw(aWidths[i])  << std::left << ' ' + aStrings[i] << View::verticalEdge;
	}
	defaultOutput << '\n';
}

} /* UrSQL */