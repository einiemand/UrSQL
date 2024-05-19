#include "statement/ExecuteResult.hpp"
#include "view/View.hpp"

namespace ursql {

ExecuteResult::ExecuteResult(std::unique_ptr<View>&& view, bool quit) : view_(std::move(view)), quit_(quit) {}

void ExecuteResult::showView(std::ostream& os) const {
    if (view_) {
        view_->show(os);
        os << "\n\n";
    }
}

bool ExecuteResult::quit() const {
    return quit_;
}

}
