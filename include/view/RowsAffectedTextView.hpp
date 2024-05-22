#include "TextView.hpp"

namespace ursql {

class RowsAffectedTextView : public TextView {
public:
    explicit RowsAffectedTextView(std::size_t numRows);
    ~RowsAffectedTextView() override = default;
};

}  // namespace ursql
