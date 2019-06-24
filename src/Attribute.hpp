#pragma once
#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP
#include "Storable.hpp"
#include "Value.hpp"

namespace UrSQL {

class Attribute : public Storable {
public:
	friend class AttributeBuilder;

	Attribute() = default;
	~Attribute() = default;

	Attribute(Attribute&&) = default;
	Attribute& operator=(const Attribute&) = delete;

	void serialize(BufferWriter& aWriter) const override;
	void deserialize(BufferReader& aReader) override;

	inline const std::string& getName() const {
		return m_name;
	}

	inline ValueType getType(ValueType aType) const {
		return m_type;
	}

	inline const Value& getDefaultValue() const {
		return m_defaultValue;
	}

	inline bool getNullable() const {
		return m_isNullable;
	}

	inline bool getPrimary() const {
		return m_isPrimary;
	}

	inline bool getAutoIncr() const {
		return m_isAutoIncr;
	}

private:
	Attribute(std::string aName, ValueType aType, Value aDefaultValue, bool aNullable, bool aPrimary, bool anAutoIncr);

	std::string m_name;
	ValueType m_type;
	Value m_defaultValue;
	bool m_isNullable;
	bool m_isPrimary;
	bool m_isAutoIncr;
};

class AttributeBuilder {
public:
	AttributeBuilder();
	~AttributeBuilder() = default;

	AttributeBuilder(AttributeBuilder&&) = default;
	AttributeBuilder& operator=(const AttributeBuilder&) = delete;
	/*    Setters    */
	inline void setName(std::string aName) {
		m_name = std::move(aName);
	}

	inline void setType(ValueType aType) {
		m_type = aType;
	}

	inline void setDefaultValue(Value aValue) {
		m_defaultValue = std::move(aValue);
	}

	inline void setNullable(bool isNullable) {
		m_isNullable = isNullable;
	}

	inline void setPrimary(bool isPrimary) {
		m_isPrimary = isPrimary;
	}

	inline void setAutoIncr(bool isAutoIncr) {
		m_isAutoIncr = isAutoIncr;
	}

	inline ValueType getType() const {
		return m_type;
	}

	inline Attribute build() const {
		return Attribute(m_name, m_type, m_defaultValue, m_isNullable, m_isPrimary, m_isAutoIncr);
	}

private:
	std::string m_name;
	ValueType m_type;
	Value m_defaultValue;
	bool m_isNullable;
	bool m_isPrimary;
	bool m_isAutoIncr;
};

} /* UrSQL */

#endif
