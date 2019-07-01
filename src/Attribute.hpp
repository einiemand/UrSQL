#pragma once
#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP
#include "Storable.hpp"
#include "Value.hpp"

namespace UrSQL {

class Attribute : public Storable {
public:
	Attribute();
	~Attribute() = default;

	Attribute(const Attribute&) = default;
	Attribute& operator=(const Attribute&) = delete;

	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline void setName(std::string aName) {
		m_name = std::move(aName);
	}

	inline void setType(ValueType aType) {
		m_type = aType;
	}

	inline void setDefaultValue(Value aValue) {
		m_defaultValue = std::move(aValue);
	}

	inline void setNullable(bool aNullable) {
		m_isNullable = aNullable;
	}

	inline void setPrimary(bool aPrimary) {
		m_isPrimary = aPrimary;
	}

	inline void setAutoIncr(bool anAutoincr) {
		m_isAutoIncr = anAutoincr;
	}

	inline const std::string& getName() const {
		return m_name;
	}

	inline ValueType getType() const {
		return m_type;
	}

	inline const Value& getDefaultValue() const {
		return m_defaultValue;
	}

	inline bool isNullable() const {
		return m_isNullable;
	}

	inline bool isPrimary() const {
		return m_isPrimary;
	}

	inline bool isAutoIncr() const {
		return m_isAutoIncr;
	}

private:
	std::string m_name;
	ValueType m_type;
	Value m_defaultValue;
	bool m_isNullable;
	bool m_isPrimary;
	bool m_isAutoIncr;
};

using AttributeList = std::vector<Attribute>;

} /* UrSQL */

#endif
