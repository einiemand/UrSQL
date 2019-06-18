#include "Value.hpp"
#include "BufferStream.hpp"

namespace UrSQL {

enum class ValueType : char {
	int_type,
	float_type,
	bool_type,
	varchar_type,
	null_type
};

class ValueBase {
public:
	using int_t = Value::int_t;
	using float_t = Value::float_t;
	using bool_t = Value::bool_t;
	using varchar_t = Value::varchar_t;
	using null_t = Value::null_t;

	virtual ValueType type() const = 0;
	virtual size_type size() const = 0;
	virtual std::unique_ptr<ValueBase> copyAndConvert(ValueType aType) const = 0;
	virtual size_type hash() const = 0;
	virtual std::string stringify() const = 0;

	virtual bool less(const ValueBase& rhs) const = 0;
	virtual bool equal(const ValueBase& rhs) const = 0;

	virtual ~ValueBase() = default;
};

template<typename T>
struct val_type_traits {};

template<>
struct val_type_traits<ValueBase::int_t> {
	static constexpr ValueType val_type = ValueType::int_type;
};

template<>
struct val_type_traits<ValueBase::float_t> {
	static constexpr ValueType val_type = ValueType::float_type;
};

template<>
struct val_type_traits<ValueBase::bool_t> {
	static constexpr ValueType val_type = ValueType::bool_type;
};

template<>
struct val_type_traits<ValueBase::varchar_t> {
	static constexpr ValueType val_type = ValueType::varchar_type;
};

template<>
struct val_type_traits<ValueBase::null_t> {
	static constexpr ValueType val_type = ValueType::null_type;
};

template<typename T>
class ValueImpl : public ValueBase {
public:
	using ValueBase::int_t;
	using ValueBase::float_t;
	using ValueBase::bool_t;
	using ValueBase::varchar_t;
	using ValueBase::null_t;

	static constexpr ValueType val_type = val_type_traits<T>::val_type;

	ValueImpl() : ValueImpl(T{}) {}

	ValueImpl(T aVal) :
		m_val(std::move(aVal))
	{
	}

	~ValueImpl() override = default;

	ValueImpl(const ValueImpl&) = default;
	ValueImpl(ValueImpl&&) = default;
	ValueImpl& operator=(const ValueImpl&) = default;
	ValueImpl& operator=(ValueImpl&&) = default;

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

	bool less(const ValueBase& rhs) const override {
		if (type() == rhs.type()) {
			return m_val < dynamic_cast<const ValueImpl<T>&>(rhs).m_val;
		}
		if (rhs.type() == ValueType::null_type) {
			return false;
		}
		throw std::runtime_error("Values of different types are not comparable");
	}

	bool equal(const ValueBase& rhs) const override {
		if (type() == rhs.type()) {
			return m_val == dynamic_cast<const ValueImpl<T>&>(rhs).m_val;
		}
		if (rhs.type() == ValueType::null_type) {
			return false;
		}
		throw std::runtime_error("Values of different types are not comparable");
	}
private:
	T m_val;
};

template<>
class ValueImpl<ValueBase::varchar_t> : public ValueBase {
public:
	using ValueBase::int_t;
	using ValueBase::float_t;
	using ValueBase::bool_t;
	using ValueBase::varchar_t;
	using ValueBase::null_t;

	static constexpr ValueType val_type = val_type_traits<varchar_t>::val_type;

	ValueImpl(varchar_t aVal = "") :
		m_val(std::move(aVal))
	{
	}

	~ValueImpl() override = default;

	ValueImpl(const ValueImpl&) = default;
	ValueImpl(ValueImpl&&) = default;
	ValueImpl& operator=(const ValueImpl&) = default;
	ValueImpl& operator=(ValueImpl&&) = default;

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

	bool less(const ValueBase& rhs) const override {
		if (type() == rhs.type()) {
			return m_val < dynamic_cast<const ValueImpl<varchar_t>&>(rhs).m_val;
		}
		if (rhs.type() == ValueType::null_type) {
			return false;
		}
		throw std::runtime_error("Values of different types are not comparable");
	}

	bool equal(const ValueBase& rhs) const override {
		if (rhs.type() == ValueType::varchar_type) {
			return m_val == dynamic_cast<const ValueImpl<varchar_t>&>(rhs).m_val;
		}
		if (rhs.type() == ValueType::null_type) {
			return false;
		}
		throw std::runtime_error("Values of different types are not comparable");
	}

private:
	varchar_t m_val;
};

template<>
class ValueImpl<ValueBase::null_t> : public ValueBase {
public:
	using ValueBase::int_t;
	using ValueBase::float_t;
	using ValueBase::bool_t;
	using ValueBase::varchar_t;
	using ValueBase::null_t;

	static constexpr ValueType val_type = val_type_traits<null_t>::val_type;

	ValueImpl() = default;

	~ValueImpl() override = default;

	ValueImpl(const ValueImpl&) = default;
	ValueImpl& operator=(const ValueImpl&) = default;

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
		return "";
	}

	bool less(const ValueBase& rhs) const override {
		return false;
	}

	bool equal(const ValueBase& rhs) const override {
		return false;
	}
};

using IntValue = ValueImpl<ValueBase::int_t>;
using FloatValue = ValueImpl<ValueBase::float_t>;
using BoolValue = ValueImpl<ValueBase::bool_t>;
using VCharValue = ValueImpl<ValueBase::varchar_t>;
using NullValue = ValueImpl<ValueBase::null_t>;

template<>
std::unique_ptr<ValueBase> ValueImpl<ValueBase::int_t>::copyAndConvert(ValueType aType) const {
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
std::unique_ptr<ValueBase> ValueImpl<ValueBase::float_t>::copyAndConvert(ValueType aType) const {
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
std::unique_ptr<ValueBase> ValueImpl<ValueBase::bool_t>::copyAndConvert(ValueType aType) const {
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

std::unique_ptr<ValueBase> ValueImpl<ValueBase::varchar_t>::copyAndConvert(ValueType aType) const {
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

std::unique_ptr<ValueBase> ValueImpl<ValueBase::null_t>::copyAndConvert(ValueType aType) const {
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
	m_Base(std::make_unique<NullValue>())
{
}

Value::Value(int_t anInt) :
	m_Base(std::make_unique<IntValue>(anInt))
{
}

Value::Value(float_t aFloat) :
	m_Base(std::make_unique<FloatValue>(aFloat))
{
}

Value::Value(bool_t aBool) :
	m_Base(std::make_unique<BoolValue>(aBool))
{
}

Value::Value(varchar_t aString) :
	m_Base(std::make_unique<VCharValue>(std::move(aString)))
{
}

Value::Value(const Value& rhs) :
	m_Base(rhs.m_Base->copyAndConvert(rhs.getType()))
{
}

Value::Value(Value&& rhs) noexcept :
	m_Base(std::move(rhs.m_Base))
{
}

Value& Value::operator=(const Value& rhs) {
	Value(rhs).swap(*this);
	return *this;
}

Value& Value::operator=(Value&& rhs) noexcept {
	swap(rhs);
	return *this;
}

void Value::serialize(BufferWriter& aWriter) const {
	aWriter << static_cast<char>(getType()) << m_Base->stringify();
}

void Value::deserialize(BufferReader& aReader) {
	char theChar;
	aReader >> theChar;
	std::string theString;
	aReader >> theString;
	Value theValue(std::move(theString));
	if (theValue.become(static_cast<ValueType>(theChar))) {
		swap(theValue);
	}
	else {
		throw std::runtime_error("Value parse error");
	}
}

ValueType Value::getType() const {
	return m_Base->type();
}

size_type Value::getSize() const {
	return m_Base->size();
}

StatusResult Value::become(ValueType aType) {
	m_Base = m_Base->copyAndConvert(aType);
	return getType() == aType ? StatusResult(Error::no_error) : StatusResult(Error::conversion_fail, "Invalid ValueType");
}

size_type Value::hash() const {
	return m_Base->hash();
}

bool operator<(const Value& lhs, const Value& rhs) {
	return lhs.m_Base->less(*rhs.m_Base);
}

bool operator==(const Value& lhs, const Value& rhs) {
	return lhs.m_Base->equal(*rhs.m_Base);
}

}