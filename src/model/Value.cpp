#include "model/Value.hpp"

#include "model/BufferStream.hpp"

namespace UrSQL {

using int_t = Value::int_t;
using float_t = Value::float_t;
using bool_t = Value::bool_t;
using varchar_t = Value::varchar_t;
using null_t = Value::null_t;

template<typename T>
struct val_type_traits {};

template<>
struct val_type_traits<int_t> {
    static constexpr ValueType val_type = ValueType::int_type;
};

template<>
struct val_type_traits<float_t> {
    static constexpr ValueType val_type = ValueType::float_type;
};

template<>
struct val_type_traits<bool_t> {
    static constexpr ValueType val_type = ValueType::bool_type;
};

template<>
struct val_type_traits<varchar_t> {
    static constexpr ValueType val_type = ValueType::varchar_type;
};

template<>
struct val_type_traits<null_t> {
    static constexpr ValueType val_type = ValueType::null_type;
};

namespace detail {

class Impl {
public:
    virtual void serialize(BufferWriter& aWriter) const = 0;
    virtual ValueType type() const = 0;
    virtual size_type ostreamSize() const = 0;
    virtual std::unique_ptr<Impl> copyAndConvert(ValueType aType) const = 0;
    virtual std::string toString() const = 0;
    virtual std::ostream& dump(std::ostream& anOutput) const = 0;

    virtual bool lt(const Impl& rhs) const = 0;
    virtual bool eq(const Impl& rhs) const = 0;

    virtual ~Impl() = default;
};

template<typename T>
class ValueImpl : public Impl {
public:
    ValueImpl() : ValueImpl(T{}) {}

    explicit ValueImpl(T aVal) : m_val(std::move(aVal)) {}

    ~ValueImpl() override = default;

    URSQL_DISABLE_COPY(ValueImpl);

    void serialize(BufferWriter& aWriter) const override {
        aWriter << type() << m_val;
    }

    ValueType type() const override {
        return val_type_traits<T>::val_type;
    }

    size_type ostreamSize() const override {
        if constexpr (std::is_same_v<T, int_t>) {
            size_type theSize = m_val <= 0 ? 1 : 0;
            for (int_t theInt = std::abs(m_val); theInt > 0; theInt /= 10) {
                ++theSize;
            }
            return theSize;
        } else if constexpr (std::is_same_v<T, float_t>) {
            return std::to_string(m_val).size();
        } else {
            URSQL_UNREACHABLE;
        }
    }

    std::unique_ptr<Impl> copyAndConvert(ValueType aType) const override;

    std::string toString() const override {
        return std::to_string(m_val);
    }

    std::ostream& dump(std::ostream& anOutput) const override {
        return anOutput << m_val;
    }

    bool lt(const Impl& rhs) const override {
        URSQL_TRUTH(type() == rhs.type(),
                    "Values of different types are not comparable");
        return m_val < dynamic_cast<const ValueImpl<T>&>(rhs).m_val;
    }

    bool eq(const Impl& rhs) const override {
        URSQL_TRUTH(type() == rhs.type(),
                    "Values of different types are not comparable");
        return m_val == dynamic_cast<const ValueImpl<T>&>(rhs).m_val;
    }

private:
    T m_val;
};

template<>
class ValueImpl<bool_t> : public Impl {
public:
    ValueImpl() : ValueImpl(bool_t{}) {}

    explicit ValueImpl(bool_t aBool) : m_val(aBool) {}

    ~ValueImpl() override = default;

    URSQL_DISABLE_COPY(ValueImpl);

    void serialize(BufferWriter& aWriter) const override {
        aWriter << type() << m_val;
    }

    ValueType type() const override {
        return val_type_traits<bool_t>::val_type;
    }

    size_type ostreamSize() const override {
        // 't' or 'f'
        return 1;
    }

    std::unique_ptr<Impl> copyAndConvert(ValueType aType) const override;

    std::string toString() const override {
        return m_val ? "t" : "f";
    }

    std::ostream& dump(std::ostream& anOutput) const override {
        return anOutput << (m_val ? 't' : 'f');
    }

    bool lt(const Impl& rhs) const override {
        URSQL_TRUTH(type() == rhs.type(),
                    "Values of different types are not comparable");
        return m_val < dynamic_cast<const ValueImpl&>(rhs).m_val;
    }

    bool eq(const Impl& rhs) const override {
        URSQL_TRUTH(type() == rhs.type(),
                    "Values of different types are not comparable");
        return m_val == dynamic_cast<const ValueImpl&>(rhs).m_val;
    }

private:
    bool_t m_val;
};

template<>
class ValueImpl<varchar_t> : public Impl {
public:
    ValueImpl() : ValueImpl(varchar_t{}) {}

    explicit ValueImpl(varchar_t aVal) : m_val(std::move(aVal)) {}

    ~ValueImpl() override = default;

    URSQL_DISABLE_COPY(ValueImpl);

    void serialize(BufferWriter& aWriter) const override {
        aWriter << type() << m_val;
    }

    ValueType type() const override {
        return val_type_traits<varchar_t>::val_type;
    }

    size_type ostreamSize() const override {
        return m_val.size();
    }

    std::unique_ptr<Impl> copyAndConvert(ValueType aType) const override;

    std::string toString() const override {
        return m_val;
    }

    std::ostream& dump(std::ostream& anOutput) const override {
        return anOutput << m_val;
    }

    bool lt(const Impl& rhs) const override {
        URSQL_TRUTH(rhs.type() == ValueType::varchar_type ||
                      rhs.type() == ValueType::null_type,
                    "Values of different types are not comparable");
        return rhs.type() == ValueType::varchar_type &&
               m_val < dynamic_cast<const ValueImpl<varchar_t>&>(rhs).m_val;
    }

    bool eq(const Impl& rhs) const override {
        URSQL_TRUTH(rhs.type() == ValueType::varchar_type ||
                      rhs.type() == ValueType::null_type,
                    "Values of different types are not comparable");
        return rhs.type() == ValueType::varchar_type &&
               m_val == dynamic_cast<const ValueImpl<varchar_t>&>(rhs).m_val;
    }

private:
    varchar_t m_val;
};

template<>
class ValueImpl<null_t> : public Impl {
public:
    ValueImpl() = default;
    ~ValueImpl() override = default;

    URSQL_DISABLE_COPY(ValueImpl);

    void serialize(BufferWriter& aWriter) const override {
        aWriter << type();
    }

    ValueType type() const override {
        return val_type_traits<null_t>::val_type;
    }

    size_type ostreamSize() const override {
        return std::char_traits<char>::length(strRep);
    }

    std::unique_ptr<Impl> copyAndConvert(ValueType aType) const override;

    std::string toString() const override {
        return strRep;
    }

    std::ostream& dump(std::ostream& anOutput) const override {
        return anOutput.write(strRep, std::char_traits<char>::length(strRep));
    }

    bool lt(const Impl&) const override {
        return false;
    }

    bool eq(const Impl&) const override {
        return false;
    }

private:
    static constexpr const char* const strRep = "NULL";
};

using IntValue = ValueImpl<int_t>;
using FloatValue = ValueImpl<float_t>;
using BoolValue = ValueImpl<bool_t>;
using VCharValue = ValueImpl<varchar_t>;
using NullValue = ValueImpl<null_t>;

template<>
std::unique_ptr<Impl> ValueImpl<int_t>::copyAndConvert(ValueType aType) const {
    switch (aType) {
    case ValueType::int_type:
        return std::make_unique<IntValue>(m_val);
    case ValueType::float_type:
        return std::make_unique<FloatValue>(static_cast<float_t>(m_val));
    case ValueType::bool_type:
        return std::make_unique<BoolValue>(static_cast<bool_t>(m_val));
    case ValueType::varchar_type:
        return std::make_unique<VCharValue>(std::to_string(m_val));
    default:
        return std::make_unique<NullValue>();
    }
}

template<>
std::unique_ptr<Impl> ValueImpl<float_t>::copyAndConvert(
  ValueType aType) const {
    switch (aType) {
    case ValueType::int_type:
        return std::make_unique<IntValue>(static_cast<int_t>(m_val));
    case ValueType::float_type:
        return std::make_unique<FloatValue>(m_val);
    case ValueType::bool_type:
        return std::make_unique<BoolValue>(static_cast<bool_t>(m_val));
    case ValueType::varchar_type:
        return std::make_unique<VCharValue>(std::to_string(m_val));
    default:
        return std::make_unique<NullValue>();
    }
}

std::unique_ptr<Impl> ValueImpl<bool_t>::copyAndConvert(ValueType aType) const {
    switch (aType) {
    case ValueType::int_type:
        return std::make_unique<IntValue>(static_cast<int_t>(m_val));
    case ValueType::float_type:
        return std::make_unique<FloatValue>(static_cast<float_t>(m_val));
    case ValueType::bool_type:
        return std::make_unique<BoolValue>(m_val);
    case ValueType::varchar_type:
        return std::make_unique<VCharValue>(
          std::to_string(static_cast<int_t>(m_val)));
    default:
        return std::make_unique<NullValue>();
    }
}

std::unique_ptr<Impl> ValueImpl<varchar_t>::copyAndConvert(
  ValueType aType) const {
    switch (aType) {
    case ValueType::int_type:
        return std::make_unique<IntValue>(std::stoi(m_val));
    case ValueType::float_type:
        return std::make_unique<FloatValue>(std::stof(m_val));
    case ValueType::bool_type:
        return std::make_unique<BoolValue>(
          static_cast<bool_t>(std::stoi(m_val)));
    case ValueType::varchar_type:
        return std::make_unique<VCharValue>(m_val);
    default:
        return std::make_unique<NullValue>();
    }
}

std::unique_ptr<Impl> ValueImpl<null_t>::copyAndConvert(ValueType aType) const {
    switch (aType) {
    case ValueType::int_type:
        return std::make_unique<IntValue>();
    case ValueType::float_type:
        return std::make_unique<FloatValue>();
    case ValueType::bool_type:
        return std::make_unique<BoolValue>();
    case ValueType::varchar_type:
        return std::make_unique<VCharValue>();
    default:
        return std::make_unique<NullValue>();
    }
}

}  // namespace detail

Value::Value() : m_impl(std::make_unique<detail::NullValue>()) {}

Value::Value(int_t anInt) : m_impl(std::make_unique<detail::IntValue>(anInt)) {}

Value::Value(float_t aFloat)
    : m_impl(std::make_unique<detail::FloatValue>(aFloat)) {}

Value::Value(bool_t aBool)
    : m_impl(std::make_unique<detail::BoolValue>(aBool)) {}

Value::Value(varchar_t aString)
    : m_impl(std::make_unique<detail::VCharValue>(std::move(aString))) {}

Value::Value(const Value& rhs)
    : m_impl(rhs.m_impl->copyAndConvert(rhs.type())) {}

Value::Value(Value&& rhs) noexcept : m_impl(std::move(rhs.m_impl)) {}

Value::~Value() = default;

Value& Value::operator=(const Value& rhs) {
    Value(rhs).swap(*this);
    return *this;
}

Value& Value::operator=(Value&& rhs) noexcept {
    swap(rhs);
    return *this;
}

void Value::serialize(BufferWriter& aWriter) const {
    m_impl->serialize(aWriter);
}

void Value::deserialize(BufferReader& aReader) {
    auto theValueType = aReader.read<ValueType>();
    switch (theValueType) {
    case ValueType::int_type:
        *this = aReader.read<int_t>();
        break;
    case ValueType::float_type:
        *this = aReader.read<float_t>();
        break;
    case ValueType::bool_type:
        *this = aReader.read<bool_t>();
        break;
    case ValueType::varchar_type:
        *this = aReader.read<varchar_t>();
        break;
    case ValueType::null_type:
        *this = Value();
        break;
    default:
        URSQL_UNREACHABLE;
    }
}

ValueType Value::type() const {
    return m_impl->type();
}

size_type Value::ostreamSize() const {
    return m_impl->ostreamSize();
}

StatusResult Value::become(ValueType aType) {
    m_impl = m_impl->copyAndConvert(aType);
    return type() == aType ?
             StatusResult(Error::no_error) :
             StatusResult(Error::conversion_fail, "Invalid ValueType");
}

std::string Value::toString() const {
    return m_impl->toString();
}

bool operator<(const Value& lhs, const Value& rhs) {
    return !lhs.isNull() && !rhs.isNull() && lhs.m_impl->lt(*rhs.m_impl);
}

bool operator==(const Value& lhs, const Value& rhs) {
    return !lhs.isNull() && !rhs.isNull() && lhs.m_impl->eq(*rhs.m_impl);
}

std::ostream& operator<<(std::ostream& anOutput, const Value& aValue) {
    return aValue.m_impl->dump(anOutput);
}

bool Value::keywordIsValueType(Keyword aKeyword) {
    switch (aKeyword) {
    case Keyword::integer_kw:
    case Keyword::float_kw:
    case Keyword::boolean_kw:
    case Keyword::varchar_kw:
        return true;
    default:
        return false;
    }
}

ValueType Value::keyword2ValueType(Keyword aKeyword) {
    switch (aKeyword) {
    case Keyword::integer_kw:
        return ValueType::int_type;
    case Keyword::float_kw:
        return ValueType::float_type;
    case Keyword::boolean_kw:
        return ValueType::bool_type;
    case Keyword::varchar_kw:
        return ValueType::varchar_type;
    default:
        URSQL_UNREACHABLE;
    }
}

}  // namespace UrSQL
