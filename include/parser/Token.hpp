#pragma once

#include <string>
#include <type_traits>
#include <variant>

#include "TokenEnums.hpp"
#include "common/Macros.hpp"

namespace ursql {

enum class TokenType {
    identifier = 0,
    keyword,
    number,
    op,
    punctuation,
    text
    //    comma,
    //    colon,
    //    number,
    //    comparator,
    //    lparen,
    //    rparen,
    //    lbracket,
    //    rbracket,
    //    lbrace,
    //    rbrace,
    //    slash,
    //    star,
    //    equal,
    //    plus,
    //    minus,
    //    keyword,
    //    identifier,
    //    string,
    //    unknown
};

template<TokenType tokenType>
struct token_type_ordinal : std::integral_constant<std::size_t, static_cast<std::size_t>(tokenType)> {};

template<TokenType tokenType>
inline constexpr const std::size_t token_type_ordinal_v = token_type_ordinal<tokenType>::value;

template<TokenType tokenType>
struct token_type_index_t {
    explicit token_type_index_t() noexcept = default;
};

template<TokenType tokenType>
inline constexpr const token_type_index_t<tokenType> token_type_index{};

class Token {
public:
    template<TokenType tokenType, typename... Args>
    explicit constexpr Token(token_type_index_t<tokenType>, Args&&... args) noexcept(std::is_nothrow_constructible_v<Var, std::in_place_index_t<token_type_ordinal_v<tokenType>>, Args...>) : var_(std::in_place_index<token_type_ordinal_v<tokenType>>, std::forward<Args>(args)...) {}

    ~Token() = default;

    URSQL_DEFAULT_COPY_CTOR(Token);
    URSQL_DEFAULT_MOVE_CTOR(Token);

    [[nodiscard]] TokenType getType() const noexcept;

    template<TokenType tokenType>
    [[nodiscard]] constexpr decltype(auto) get() const noexcept {
        return std::get<token_type_ordinal_v<tokenType>>(var_);
    }

    template<TokenType tokenType>
    [[nodiscard]] constexpr decltype(auto) get_if() const noexcept {
        return std::get_if<token_type_ordinal_v<tokenType>>(&var_);
    }

    [[nodiscard]] bool isKeyword(Keyword keyword) const noexcept;
    [[nodiscard]] bool isPunctuation(Punctuation punctuation) const noexcept;

private:
    using Var = std::variant<std::string, Keyword, float, Operator, Punctuation, std::string>;

    const Var var_;

    template<TokenType tokenType>
    struct var_alt {
        using type = std::variant_alternative_t<token_type_ordinal_v<tokenType>, Var>;
    };

    template<TokenType tokenType>
    using var_alt_t = typename var_alt<tokenType>::type;

    static_assert(std::is_same_v<std::string, var_alt_t<TokenType::identifier>>, "Identifier token should be stored as string");
    static_assert(std::is_same_v<Keyword, var_alt_t<TokenType::keyword>>, "Keyword token should be stored as Keyword");
    static_assert(std::is_same_v<float, var_alt_t<TokenType::number>>, "Number token should be stored as float");
    static_assert(std::is_same_v<Operator, var_alt_t<TokenType::op>>, "Operator token should be stored as Operator");
    static_assert(std::is_same_v<Punctuation, var_alt_t<TokenType::punctuation>>, "Punctuation token should be stored as Punctuation");
    static_assert(std::is_same_v<std::string, var_alt_t<TokenType::text>>, "Text token should be stored as string");
};

static_assert(std::is_same_v<const std::string&, decltype(std::declval<Token>().get<TokenType::identifier>())>);

}  // namespace ursql
