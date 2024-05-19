#pragma once

#include <iostream>

namespace ursql {

class View {
public:
    explicit View() = default;
    virtual ~View() = default;

    virtual void show(std::ostream& os) const = 0;
};

class TextView : public View {
public:
    explicit TextView(std::string text);
    ~TextView() override = default;

    void show(std::ostream& os) const override;

private:
    std::string text_;
};

}
