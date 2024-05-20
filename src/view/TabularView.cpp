#include "view/TabularView.hpp"
#include "exception/InternalError.hpp"

namespace ursql {

namespace {

std::vector<std::size_t> calculateColumnWidth(const std::vector<std::string>& headers, const std::vector<std::vector<Value>>& valueRows) {
    std::vector<std::size_t> widths;
    widths.reserve(headers.size());
    for (auto& header : headers) {
        widths.push_back(header.length());
    }
    for (auto& valueRow : valueRows) {
        URSQL_ASSERT(valueRow.size() == headers.size(), "row size should match header size");
        for (std::size_t i = 0; i < valueRow.size(); ++i) {
            widths[i] = std::max(widths[i], valueRow[i].displayWidth());
        }
    }
    return widths;
}

}

TabularView::TabularView(std::vector<std::string> headers, std::vector<std::vector<Value>> valueRows) : headers_(std::move(headers)), valueRows_(std::move(valueRows)) {}

void TabularView::show(std::ostream& os) const {
    if (valueRows_.empty()) {
        os << "empty set";
        return;
    }
    std::vector<std::size_t> widths = calculateColumnWidth(headers_, valueRows_);
    _printBreak(os, widths);
    _printHeaders(os, widths);
    _printBreak(os, widths);
    _printValueRows(os, widths);
    _printBreak(os, widths);
    os << valueRows_.size() << " row(s) in set";
}

void TabularView::_printBreak(std::ostream& os, const std::vector<std::size_t>& widths) {
    os << '+';
    auto fill = os.fill('-');
    for (std::size_t width : widths) {
        os << std::setw(static_cast<int>(width) + 3) << '+';
    }
    os.fill(fill);
    os << '\n';
}

void TabularView::_printHeaders(std::ostream& os, const std::vector<std::size_t>& widths) const {
    os << '|';
    for (std::size_t i = 0; i < headers_.size(); ++i) {
        os << std::setw(static_cast<int>(widths[i]) + 1) << headers_[i] << " |";
    }
    os << '\n';
}

void TabularView::_printValueRows(std::ostream& os, const std::vector<std::size_t>& widths) const {
    for (auto& valueRow : valueRows_) {
        os << '|';
        for (std::size_t i = 0; i < valueRow.size(); ++i) {
            os << std::setw(static_cast<int>(widths[i]) + 1) << valueRow[i] << " |";
        }
        os << '\n';
    }
}

}
