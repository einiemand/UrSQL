#include "statement/Filter.hpp"

#include "model/Row.hpp"
#include "parser/TokenStream.hpp"
#include "exception/UserError.hpp"

namespace ursql {

class Operand {
public:

    static Operand parse(TokenStream& ts) {
        URSQL_EXPECT(ts.hasNext(), MissingInput, "operand");
        auto& token = ts.peek();
        switch (token.getType()) {
        case TokenType::identifier:
            return
        }
    }
private:
    std::variant<std::string, Value> var_;

    struct IdentifierTag {};
    struct ValueTag {};
    Operand(std::string identifier)
};

class Expr {
public:

private:

};

class AndFilter : public Filter {
public:
    explicit AndFilter(std::vector<std::unique_ptr<Filter>> filters) : Filter(), filters_(std::move(filters)) {}
    ~AndFilter() override = default;

    [[nodiscard]] bool eval(const Entity& entity, const Row& row) const override {
        return std::ranges::all_of(filters_, [&](auto& filter) { return filter->eval(entity, row); });
    }
private:
    const std::vector<std::unique_ptr<Filter>> filters_;
};

}
