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

class ValueBase {
public:
    virtual ValueType type() const = 0;
    virtual size_type size() const = 0;
    virtual std::unique_ptr<ValueBase> copyAndConvert(ValueType aType) const = 0;
    virtual size_type hash() const = 0;
    virtual std::string stringify() const = 0;
    virtual std::ostream& dump(std::ostream& anOutput) const = 0;

    virtual bool less(const ValueBase& rhs) const = 0;
    virtual bool equal(const ValueBase& rhs) const = 0;

    virtual ~ValueBase() = default;
};

template<typename T>
class ValueImpl : public ValueBase {
public:
	static constexpr ValueType val_type = val_type_traits<T>::val_type;

	ValueImpl() : ValueImpl(T{}) {}

	ValueImpl(T aVal) :
		m_val(std::move(aVal))
	{
	}

	~ValueImpl() override = default;

	URSQL_DEFAULT_COPY(ValueImpl);
	URSQL_DEFAULT_MOVE(ValueImpl);

	ValueType type() const override {
		return val_type;
	}

	size_type size() const override {
		return sizeof(T);
	}

	std::unique_ptr<ValueBase> copyAndConvert(ValueType aType) const override;

	size_type hash() const override {
		return std::hash<T>()(m_val);
	}

	std::string stringify() const override {
		return std::to_string(m_val);
	}

	std::ostream& dump(std::ostream& anOutput) const override {
		return anOutput << m_val;
	}

	bool less(const ValueBase& rhs) const override {
		URSQL_TRUTH(type() == rhs.type(), "Values of different types are not comparable");
		return m_val < dynamic_cast<const ValueImpl<T>&>(rhs).m_val;
	}

	bool equal(const ValueBase& rhs) const override {
		URSQL_TRUTH(type() == rhs.type(), "Values of different types are not comparable");
		return m_val == dynamic_cast<const ValueImpl<T>&>(rhs).m_val;
	}
private:
	T m_val;
};

template<>
class ValueImpl<varchar_t> : public ValueBase {
public:
	static constexpr ValueType val_type = val_type_traits<varchar_t>::val_type;

	ValueImpl(varchar_t aVal = "") :
		m_val(std::move(aVal))
	{
	}

	~ValueImpl() override = default;

	URSQL_DEFAULT_COPY(ValueImpl);
	URSQL_DEFAULT_MOVE(ValueImpl);

	ValueType type() const override {
		return val_type;
	}

	size_type size() const override {
		return m_val.size();
	}

	std::unique_ptr<ValueBase> copyAndConvert(ValueType aType) const override;

	size_type hash() const override {
		return std::hash<varchar_t>()(m_val);
	}

	std::string stringify() const override {
		return m_val;
	}

	std::ostream& dump(std::ostream& anOutput) const override {
		return anOutput << m_val;
	}

	bool less(const ValueBase& rhs) const override {
		URSQL_TRUTH(rhs.type() == ValueType::varchar_type || rhs.type() == ValueType::null_type,
			"Values of different types are not comparable");
		return rhs.type() == ValueType::varchar_type && m_val < dynamic_cast<const ValueImpl<varchar_t>&>(rhs).m_val;
	}

	bool equal(const ValueBase& rhs) const override {
		URSQL_TRUTH(rhs.type() == ValueType::varchar_type || rhs.type() == ValueType::null_type,
			"Values of different types are not comparable");
		return rhs.type() == ValueType::varchar_type && m_val == dynamic_cast<const ValueImpl<varchar_t>&>(rhs).m_val;
	}

private:
	varchar_t m_val;
};

template<>
class ValueImpl<null_t> : public ValueBase {
public:

	static constexpr ValueType val_type = val_type_traits<null_t>::val_type;

	ValueImpl() = default;

	~ValueImpl() override = default;

	URSQL_DEFAULT_COPY(ValueImpl);

	ValueType type() const override {
		return val_type;
	}

	size_type size() const override {
		return 0;
	}

	std::unique_ptr<ValueBase> copyAndConvert(ValueType aType) const override;

	size_type hash() const override {
		return 0;
	}

	std::string stringify() const override {
		return "NULL";
	}

	std::ostream& dump(std::ostream& anOutput) const override {
		return anOutput << "NULL";
	}

	bool less(const ValueBase&) const override {
		return false;
	}

	bool equal(const ValueBase&) const override {
		return false;
	}
};

using IntValue = ValueImpl<int_t>;
using FloatValue = ValueImpl<float_t>;
using BoolValue = ValueImpl<bool_t>;
using VCharValue = ValueImpl<varchar_t>;
using NullValue = ValueImpl<null_t>;

template<>
std::unique_ptr<ValueBase> ValueImpl<int_t>::copyAndConvert(ValueType aType) const {
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
std::unique_ptr<ValueBase> ValueImpl<float_t>::copyAndConvert(ValueType aType) const {
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

template<>
std::unique_ptr<ValueBase> ValueImpl<bool_t>::copyAndConvert(ValueType aType) const {
	switch (aType) {
	case ValueType::int_type:
		return std::make_unique<IntValue>(static_cast<int_t>(m_val));
	case ValueType::float_type:
		return std::make_unique<FloatValue>(static_cast<float_t>(m_val));
	case ValueType::bool_type:
		return std::make_unique<BoolValue>(m_val);
	case ValueType::varchar_type:
		return std::make_unique<VCharValue>(std::to_string(static_cast<int_t>(m_val)));
	default:
		return std::make_unique<NullValue>();
	}
}

std::unique_ptr<ValueBase> ValueImpl<varchar_t>::copyAndConvert(ValueType aType) const {
	switch (aType) {
	case ValueType::int_type:
		return std::make_unique<IntValue>(std::stoi(m_val));
	case ValueType::float_type:
		return std::make_unique<FloatValue>(std::stof(m_val));
	case ValueType::bool_type:
		return std::make_unique<BoolValue>(static_cast<bool_t>(std::stoi(m_val)));
	case ValueType::varchar_type:
		return std::make_unique<VCharValue>(m_val);
	default:
		return std::make_unique<NullValue>();
	}
}

std::unique_ptr<ValueBase> ValueImpl<null_t>::copyAndConvert(ValueType aType) const {
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

Value::Value() :
	m_base(std::make_unique<NullValue>())
{
}

Value::Value(int_t anInt) :
	m_base(std::make_unique<IntValue>(anInt))
{
}

Value::Value(float_t aFloat) :
	m_base(std::make_unique<FloatValue>(aFloat))
{
}

Value::Value(bool_t aBool) :
	m_base(std::make_unique<BoolValue>(aBool))
{
}

Value::Value(varchar_t aString) :
	m_base(std::make_unique<VCharValue>(std::move(aString)))
{
}

Value::Value(const Value& rhs) :
	m_base(rhs.m_base->copyAndConvert(rhs.getType()))
{
}

Value::Value(Value&& rhs) noexcept :
	m_base(std::move(rhs.m_base))
{
}

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
	aWriter << static_cast<char>(getType()) << m_base->stringify();
}

void Value::deserialize(BufferReader& aReader) {
	char theChar;
	aReader >> theChar;
	std::string theString;
	aReader >> theString;
	Value theValue(std::move(theString));
	StatusResult theResult = theValue.become(static_cast<ValueType>(theChar));
	URSQL_TRUTH(theResult, "Value parse error");
	swap(theValue);
}

ValueType Value::getType() const {
	return m_base->type();
}

size_type Value::getSize() const {
	return m_base->size();
}

StatusResult Value::become(ValueType aType) {
	m_base = m_base->copyAndConvert(aType);
	return getType() == aType ? StatusResult(Error::no_error) : StatusResult(Error::conversion_fail, "Invalid ValueType");
}

size_type Value::hash() const {
	return m_base->hash();
}

std::string Value::stringify() const {
    return m_base->stringify();
}

bool operator<(const Value& lhs, const Value& rhs) {
	return !lhs.isNull() && !rhs.isNull() && lhs.m_base->less(*rhs.m_base);
}

bool operator==(const Value& lhs, const Value& rhs) {
	return !lhs.isNull() && !rhs.isNull() && lhs.m_base->equal(*rhs.m_base);
}

std::ostream& operator<<(std::ostream& anOutput, const Value& aValue) {
	return aValue.m_base->dump(anOutput);
}

static const std::unordered_map<Keyword, ValueType> str2type{
		{ Keyword::integer_kw,ValueType::int_type },
		{ Keyword::float_kw,ValueType::float_type },
		{ Keyword::boolean_kw,ValueType::bool_type },
		{ Keyword::varchar_kw,ValueType::varchar_type }
};

bool Value::keywordIsValueType(Keyword aKeyword) {
	return str2type.count(aKeyword) == 1;
}

ValueType Value::keyword2ValueType(Keyword aKeyword) {
	return str2type.at(aKeyword);
}

}