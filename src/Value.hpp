#pragma once
#ifndef VALUE_HPP
#define VALUE_HPP
#include "Storable.hpp"
#include "Keyword.hpp"
#include <memory>

namespace UrSQL {

enum class ValueType : char {
	int_type,
	float_type,
	bool_type,
	varchar_type,
	null_type
};

namespace {

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

}

class Value : public Storable {
public:
	using int_t = typename size_trait<sizeof(void*)>::int_t;
	using float_t = float_t;
	using bool_t = bool;
	using varchar_t = std::string;
	using null_t = void;

	Value();
	Value(int_t anInt);
	Value(float_t aFloat);
	Value(bool_t aBool);
	Value(varchar_t aString);
	~Value() override = default;

	Value(const Value& rhs);
	Value(Value&& rhs) noexcept;
	Value& operator=(const Value& rhs);
	Value& operator=(Value&& rhs) noexcept;

	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline void swap(Value& rhs) noexcept {
		m_base.swap(rhs.m_base);
	}

	ValueType getType() const;
	size_type getSize() const;
	StatusResult become(ValueType aType);
	size_type hash() const;

	friend bool operator<(const Value& lhs, const Value& rhs);
	friend bool operator==(const Value& lhs, const Value& rhs);
	friend std::ostream& operator<<(std::ostream& anOutput, const Value& aValue);

	static bool keywordIsValueType(Keyword aKeyword);
	static ValueType keyword2ValueType(Keyword aKeyword);

private:
	std::unique_ptr<ValueBase> m_base;
};

}

#endif /* VALUE_HPP */
