#pragma once

#include "common/Error.hpp"
#include <string>

namespace UrSQL {

class Entity;

class Order {
public:
	Order();
	~Order() = default;

	URSQL_DISABLE_COPY(Order);

	StatusResult validate(const Entity& anEntity) const;

	void setFieldName(std::string aFieldName);
	void setDesc(bool aDesc);

	inline const std::string& getFieldName() const {
		return m_fieldName;
	}

	inline bool isDesc() const {
		return m_desc;
	}
private:
	std::string m_fieldName;
	bool m_desc;
};

} /* UrSQL */
