#pragma once

#include <memory>

#include "view/View.hpp"

namespace ursql {

class ExecuteResult {
public:
    ExecuteResult(std::unique_ptr<View>&& view, bool quit);
    ~ExecuteResult() = default;

    void showView(std::ostream& os) const;
    [[nodiscard]] bool quit() const;

private:
    std::unique_ptr<View> view_;
    bool quit_;
};

}  // namespace ursql
