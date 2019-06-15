#include "View.hpp"
#include "FolderReader.hpp"
#include "Block.hpp"
#include <algorithm>
#include <iomanip>

namespace UrSQL {

/* -------------------------------ShowDBView------------------------------- */
ShowDBView::ShowDBView(const StringList& aDBNames) :
	View(),
	m_dbnames(aDBNames)
{
}

void ShowDBView::show() const {
	static const std::string theHeader = "Database";
	size_type theHorizontalWidth = theHeader.length();

	for (const auto& theDBName : m_dbnames) {
		theHorizontalWidth = std::max(theHorizontalWidth, theDBName.length());
	}
	theHorizontalWidth += 2;

	const std::vector<size_type> theWidths({ theHorizontalWidth });

	View::print_horizontal_line(theWidths);
	View::print_line({ theHeader }, theWidths);
	View::print_horizontal_line(theWidths);

	for (const auto& theDBName : m_dbnames) {
		View::print_line({ theDBName }, theWidths);
	}

	View::print_horizontal_line(theWidths);
}

/* -------------------------------View Static Methods------------------------------- */
void View::print_horizontal_line(const std::vector<size_type>& aWidths) {
	defaultOutput << View::vertex;
	for (size_type aWidth : aWidths) {
		defaultOutput << std::string(aWidth, View::horizontal_edge) << View::vertex;
	}
	defaultOutput << '\n';
}

void View::print_line(const StringList& aStrings, const std::vector<size_type>& aWidths) {
	if (aStrings.size() != aWidths.size()) {
		throw std::runtime_error("StringList size should be equal to aWidths size!");
	}
	size_type i = 0;
	defaultOutput << View::vertical_edge;
	for (; i < aStrings.size(); ++i) {
		defaultOutput << std::setw(aWidths[i])  << std::left << ' ' + aStrings[i] << View::vertical_edge;
	}
	defaultOutput << '\n';
}

DescDBView::DescDBView(const std::vector<BlockType>& theTypes) :
	m_types(theTypes)
{
}

void DescDBView::show() const {
	static const std::string theFirstHeader = "Block", theSecondHeader = "Type";
	static const size_type theFirstWidth = theFirstHeader.length() + 2;
	static constexpr size_type theSecondWidth = 6 + 2;

	View::print_horizontal_line({ theFirstWidth,theSecondWidth });
	View::print_line({ theFirstHeader,theSecondHeader }, { theFirstWidth,theSecondWidth });
	View::print_horizontal_line({ theFirstWidth,theSecondWidth });

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
		View::print_line({ std::to_string(i),theTypeString }, { theFirstWidth,theSecondWidth });
	}
	View::print_horizontal_line({ theFirstWidth,theSecondWidth });
}

} /* UrSQL */