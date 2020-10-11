#include "model/Order.hpp"

#include "model/Entity.hpp"

namespace UrSQL {

Order::Order() : m_fieldName(), m_desc(false) {}

StatusResult Order::validate(const Entity& anEntity) const {
    return anEntity.attributeExistsByName(m_fieldName) ?
             StatusResult(Error::no_error) :
             StatusResult(Error::unknown_attribute, '\'' + m_fieldName + '\'');
}

void Order::setFieldName(std::string aFieldName) {
    m_fieldName = std::move(aFieldName);
}

void Order::setDesc(bool aDesc) {
    m_desc = aDesc;
}

}  // namespace UrSQL
