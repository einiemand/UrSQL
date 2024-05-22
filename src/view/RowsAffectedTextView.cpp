#include "view/RowsAffectedTextView.hpp"

#include <format>

namespace ursql {

namespace {

std::string num2RowsAffectedText(std::size_t numRows) {
    return std::format("Query OK, {} row{} affected", numRows,
                       numRows > 1 ? "s" : "");
}

}  // namespace

RowsAffectedTextView::RowsAffectedTextView(std::size_t numRows)
    : TextView(num2RowsAffectedText(numRows)) {}

}  // namespace ursql
