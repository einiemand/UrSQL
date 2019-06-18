#pragma once
#ifndef VALUE_HPP
#define VALUE_HPP
#include "Storable.hpp"
#include <memory>

namespace UrSQL {

enum class ValueType : char;

class ValueBase;

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

private:
	std::unique_ptr<ValueBase> m_base;
};

}

#endif /* VALUE_HPP */
