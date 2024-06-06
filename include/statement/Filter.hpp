#pragma once

#include <memory>

namespace ursql {

class TokenStream;
class Entity;
class Row;

class Filter {
public:
    explicit Filter() = default;
    virtual ~Filter() = 0;
    [[nodiscard]] virtual bool eval(const Entity& entity, const Row& row) const = 0;

    static std::unique_ptr<Filter> parse(TokenStream& ts);
};

}
