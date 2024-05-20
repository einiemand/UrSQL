#pragma once

#include "view/View.hpp"

namespace ursql {

class TextView : public View {
public:
    explicit TextView(std::string text);
    ~TextView() override = default;

    void show(std::ostream& os) const override;

private:
    const std::string text_;
};

}  // namespace ursql
