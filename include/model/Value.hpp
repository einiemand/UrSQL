#pragma once

#include <memory>

#include "Keyword.hpp"
#include "Storable.hpp"

namespace ursql {

enum class ValueType : char {
    int_type,
    float_type,
    bool_type,
    varchar_type,
    null_type
};

namespace detail {

class Impl;

}

class Value : public Storable {
public:
    using int_t = typename size_trait<sizeof(void*)>::int_t;
    using float_t = float;
    using bool_t = bool;
    using varchar_t = std::string;
    using null_t = void;

    Value();
    Value(int_t anInt);
    Value(float_t aFloat);
    Value(bool_t aBool);
    Value(varchar_t aString);
    ~Value() override;

    Value(const Value& rhs);
    Value(Value&& rhs) noexcept;
    Value& operator=(const Value& rhs);
    Value& operator=(Value&& rhs) noexcept;

    void serialize(BufferWriter& aWriter) const override;
    void deserialize(BufferReader& aReader) override;

    inline void swap(Value& rhs) noexcept {
        std::swap(m_impl, rhs.m_impl);
    }

    ValueType type() const;
    size_type ostreamSize() const;
    StatusResult become(ValueType aType);

    inline bool isNull() const {
        return type() == ValueType::null_type;
    }

    std::string toString() const;

    friend bool operator<(const Value& lhs, const Value& rhs);
    friend bool operator==(const Value& lhs, const Value& rhs);
    friend std::ostream& operator<<(std::ostream& anOutput,
                                    const Value& aValue);

    static bool keywordIsValueType(Keyword aKeyword);
    static ValueType keyword2ValueType(Keyword aKeyword);

private:
    std::unique_ptr<detail::Impl> m_impl;
};

inline bool operator!=(const Value& lhs, const Value& rhs) {
    return !(lhs == rhs);
}

inline bool operator<=(const Value& lhs, const Value& rhs) {
    return lhs < rhs || lhs == rhs;
}

inline bool operator>(const Value& lhs, const Value& rhs) {
    return rhs < lhs;
}

inline bool operator>=(const Value& lhs, const Value& rhs) {
    return rhs <= lhs;
}

}  // namespace ursql
