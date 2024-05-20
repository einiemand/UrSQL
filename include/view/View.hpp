#pragma once

#include <ostream>

#include "common/Macros.hpp"

namespace ursql {

class View {
public:
    explicit View() = default;
    virtual ~View() = default;

    URSQL_DISABLE_COPY(View);

    virtual void show(std::ostream& os) const = 0;
};

}  // namespace ursql
