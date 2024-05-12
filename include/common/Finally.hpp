#pragma once

#include "Macros.hpp"

namespace ursql {

template<typename F>
class Finally {
public:
    explicit Finally(F&& f) : f_(std::forward<F>(f)) {}
    ~Finally() {
        std::invoke(f_);
    }

    URSQL_DISABLE_COPY(Finally);
private:
    F f_;
};

}
