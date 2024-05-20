#pragma once
#include <sstream>

#include "TokenStream.hpp"
#include "common/Macros.hpp"

namespace ursql {

class SQLBlob {
public:
    explicit SQLBlob() = default;
    ~SQLBlob() = default;

    URSQL_DISABLE_COPY(SQLBlob);
    URSQL_DEFAULT_MOVE(SQLBlob);

    [[nodiscard]] bool ready() const;

    [[nodiscard]] TokenStream tokenize();

    friend std::istream& operator>>(std::istream& input, SQLBlob& blob);

private:
    std::stringstream buf_;
    bool ready_ = false;
    bool inSingleQuote_ = false;
    bool inDoubleQuote_ = false;

    static constexpr const char delim = ';';

    using CharPredicate = std::function<bool(char)>;

    std::string _readUntil(const CharPredicate& pred);
    std::string _readUntil(char c);
    std::string _readWhile(const CharPredicate& pred);
    void _reset();
};

}  // namespace ursql
