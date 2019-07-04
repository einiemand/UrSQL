#include "Entity.hpp"
#include "BufferStream.hpp"
#include "Row.hpp"

namespace UrSQL {

Entity::Entity(blocknum_t aBlocknum) :
	MonoStorable(aBlocknum),
	m_dirty(false),
	m_attributes(),
	m_autoincr(0),
	m_rowPos()
{
}

BlockType Entity::expectedBlockType() const {
	return BlockType::entity_type;
}

void Entity::serialize(BufferWriter& aWriter) const {
	aWriter << static_cast<size_type>(m_attributes.size());
	for (const Attribute& theAttr : m_attributes) {
		aWriter << theAttr;
	}
	aWriter << m_autoincr;
	aWriter << static_cast<size_type>(m_rowPos.size());
	for (blocknum_t theBlocknum : m_rowPos) {
		aWriter << theBlocknum;
	}
}

void Entity::deserialize(BufferReader& aReader) {
	size_type theAttrCount;
	aReader >> theAttrCount;
	for (; theAttrCount > 0; --theAttrCount) {
		Attribute theAttr;
		aReader >> theAttr;
		m_attributes.emplace_back(std::move(theAttr));
	}
	aReader >> m_autoincr;
	size_type theRowCount;
	aReader >> theRowCount;
	for (; theRowCount > 0; --theRowCount) {
		blocknum_t theBlocknum;
		aReader >> theBlocknum;
		m_rowPos.insert(theBlocknum);
	}
}

void Entity::addAttribute(Attribute anAttribute) {
	m_attributes.emplace_back(std::move(anAttribute));
	setDirty(true);
}

bool Entity::attributeExistsByName(const std::string& aName) const {
	return std::find_if(m_attributes.cbegin(), m_attributes.cend(),
		[&aName](const auto& anAttribute)->bool {
			return anAttribute.getName() == aName;
		}
	) != m_attributes.cend();
}

const Attribute& Entity::getAttributeByName(const std::string& aName) const {
	if (attributeExistsByName(aName)) {
		return *std::find_if(m_attributes.cbegin(), m_attributes.cend(),
			[&aName](const auto& anAttribute)->bool {
				return anAttribute.getName() == aName;
			}
		);
	}
	throw std::runtime_error("Check if attribute exists before getting it!");
}

Entity::int_t Entity::getNextAutoincr() {
	setDirty(true);
	return m_autoincr++;
}

void Entity::addRowPosition(blocknum_t aBlocknum) {
	if (m_rowPos.count(aBlocknum)) {
		throw std::runtime_error("Impossible: attempting to add a new row position that's ALREADY recorded");
	}
	m_rowPos.insert(aBlocknum);
}

void Entity::dropRowPosition(blocknum_t aBlocknum) {
	if (!m_rowPos.count(aBlocknum)) {
		throw std::runtime_error("Impossible: attempting to drop a row position that's NOT recorded");
	}
	m_rowPos.erase(aBlocknum);
}

StatusResult Entity::generateNewRow(Row& aRow, const StringList& aFieldNames, const StringList& aValueStrs) {
	StatusResult theResult(Error::no_error);
	for (size_type i = 0; theResult && i < aFieldNames.size(); ++i) {
		const std::string& theFieldName = aFieldNames[i];
		if (attributeExistsByName(theFieldName)) {
			const Attribute& theAttribute = getAttributeByName(theFieldName);
			const std::string& theValueStr = aValueStrs[i];
			Value theValue(theValueStr);
			if (theResult = theValue.become(theAttribute.getType())) {
				aRow.addField(theFieldName, std::move(theValue));
			}
		}
		else{
			theResult.setError(Error::unknown_attribute, '\'' + theFieldName + '\'');
		}
	}

	// Validate all required fields are given.
	for (const Attribute& theAttribute : getAttributes()) {
		const std::string& theAttributeName = theAttribute.getName();
		if (!aRow.fieldExists(theAttributeName) && !theAttribute.isNullable()) {
			theResult.setError(Error::invalid_arguments, '\'' + theAttributeName + "' is NOT nullable, but value is not given");
			break;
		}
	}

	// Add fields that are not specified by user.
	for (auto iter = m_attributes.cbegin(); theResult && iter != m_attributes.cend(); ++iter) {
		const auto& theAttribute = *iter;
		const std::string& theAttributeName = iter->getName();
		if (!aRow.fieldExists(theAttributeName)) {
			if (theAttribute.isAutoIncr()) {
				aRow.addField(theAttributeName, Value(getNextAutoincr()));
			}
			else if (theAttribute.isNullable()) {
				aRow.addField(theAttributeName, theAttribute.getDefaultValue());
			}
			else {
				throw std::runtime_error("Impossible: the attribute must be auto_increment or nullable!");
			}
		}
	}
	return theResult;
}

}