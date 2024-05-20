#include "view/TextView.hpp"

namespace ursql {

TextView::TextView(std::string text) : View(), text_(std::move(text)) {}

void TextView::show(std::ostream& os) const {
    os << text_;
}

}  // namespace ursql
