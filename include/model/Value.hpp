#pragma once

#include <memory>
#include <tuple>

#include "Storable.hpp"
#include "parser/TokenEnums.hpp"
#include <string>

namespace ursql {

enum class ValueType : std::size_t {
    null_type,
    int_type,
    float_type,
    bool_type,
    varchar_type,
};

namespace {

template<ValueType valueType>
struct val_type_ordinal
    : std::integral_constant<std::size_t, static_cast<std::size_t>(valueType)> {
};

template<ValueType valueType>
inline constexpr const std::size_t val_type_ordinal_v =
  val_type_ordinal<valueType>::value;

}  // namespace

class Value : public Storable {
public:
    using null_t = std::monostate;
    using int_t = int;
    using float_t = float;
    using bool_t = bool;
    using varchar_t = std::string;

    explicit Value() noexcept(
      std::is_nothrow_constructible_v<
        Var, std::in_place_index_t<val_type_ordinal_v<ValueType::null_type>>,
        null_t>) = default;
    explicit Value(int_t intVal) noexcept(
      std::is_nothrow_constructible_v<
        Var, std::in_place_index_t<val_type_ordinal_v<ValueType::int_type>>,
        int_t>);
    explicit Value(float_t floatVal) noexcept(
      std::is_nothrow_constructible_v<
        Var, std::in_place_index_t<val_type_ordinal_v<ValueType::float_type>>,
        float_t>);
    explicit Value(bool_t boolVal) noexcept(
      std::is_nothrow_constructible_v<
        Var, std::in_place_index_t<val_type_ordinal_v<ValueType::bool_type>>,
        bool_t>);
    explicit Value(varchar_t varcharVal) noexcept(
      std::is_nothrow_constructible_v<
        Var, std::in_place_index_t<val_type_ordinal_v<ValueType::varchar_type>>,
        std::add_rvalue_reference_t<varchar_t>>);

    ~Value() override = default;

    constexpr Value(const Value& rhs) noexcept(
      std::is_nothrow_copy_constructible_v<Var>) = default;
    constexpr Value(Value&& rhs) noexcept(
      std::is_nothrow_move_constructible_v<Var>) = default;
    Value& operator=(const Value& rhs) noexcept(
      std::is_nothrow_copy_assignable_v<Var>) = default;
    Value& operator=(Value&& rhs) noexcept(
      std::is_nothrow_move_assignable_v<Var>) = default;

    [[nodiscard]] ValueType getType() const;
    [[nodiscard]] std::string toString() const;
    [[nodiscard]] std::size_t displayWidth() const;
    void show(std::ostream& os) const;

    void serialize(BufferWriter& aWriter) const override;
    void deserialize(BufferReader& aReader) override;

    friend bool operator<(const Value& lhs, const Value& rhs);
    friend bool operator==(const Value& lhs, const Value& rhs);

    static bool keywordIsValueType(Keyword aKeyword);
    static ValueType keyword2ValueType(Keyword aKeyword);

private:
    using Var = std::variant<null_t, int_t, float_t, bool_t, varchar_t>;

    Var var_;

    template<ValueType valueType>
    struct var_alt {
        using type =
          std::variant_alternative_t<val_type_ordinal_v<valueType>, Var>;
    };

    template<ValueType valueType>
    using var_alt_t = typename var_alt<valueType>::type;

    static_assert(std::is_same_v<null_t, var_alt_t<ValueType::null_type>>,
                  "null type should match variant index");
    static_assert(std::is_same_v<int_t, var_alt_t<ValueType::int_type>>,
                  "int type should match variant index");
    static_assert(std::is_same_v<float_t, var_alt_t<ValueType::float_type>>,
                  "float type should match variant index");
    static_assert(std::is_same_v<bool_t, var_alt_t<ValueType::bool_type>>,
                  "bool type should match variant index");
    static_assert(std::is_same_v<varchar_t, var_alt_t<ValueType::varchar_type>>,
                  "varchar type should match variant index");
};

std::ostream& operator<<(std::ostream& os, const Value& val);

}  // namespace ursql
