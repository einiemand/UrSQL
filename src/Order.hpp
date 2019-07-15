#pragma once
#ifndef ORDER_HPP
#define ORDER_HPP
#include "Error.hpp"
#include <string>

namespace UrSQL {

class Entity;

class Order {
public:
	Order();
	~Order() = default;

	Order(const Order&) = delete;
	Order& operator=(const Order&) = delete;

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

#endif /* Order.hpp */