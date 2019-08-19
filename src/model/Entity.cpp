#include "model/Entity.hpp"
#include "model/BufferStream.hpp"
#include "model/Row.hpp"

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
		m_rowPos.push_back(theBlocknum);
	}
}

void Entity::addAttribute(Attribute anAttribute) {
	m_attributes.emplace_back(std::move(anAttribute));
	makeDirty(true);
}

bool Entity::attributeExistsByName(const std::string& aName) const {
	return std::find_if(m_attributes.cbegin(), m_attributes.cend(),
		[&aName](const auto& anAttribute)->bool {
			return anAttribute.getName() == aName;
		}
	) != m_attributes.cend();
}

const Attribute& Entity::getAttributeByName(const std::string& aName) const {
	URSQL_TRUTH(attributeExistsByName(aName), "Check if attribute exists before getting it!");
	return *std::find_if(m_attributes.cbegin(), m_attributes.cend(),
		[&aName](const auto& anAttribute)->bool {
			return anAttribute.getName() == aName;
		}
	);
}

Entity::int_t Entity::getNextAutoincr() {
	makeDirty(true);
	return m_autoincr++;
}

void Entity::addRowPosition(blocknum_t aBlocknum) {
	URSQL_TRUTH(!_blocknumExists(aBlocknum), "Attempting to add a new row position that's ALREADY recorded");
	m_rowPos.push_back(aBlocknum);
	makeDirty(true);
}

void Entity::dropRowPosition(blocknum_t aBlocknum) {
	URSQL_TRUTH(_blocknumExists(aBlocknum), "Attempting to drop a row position that's NOT recorded");
	m_rowPos.erase(m_rowPos.cbegin() + _indexOfBlocknum(aBlocknum));
	makeDirty(true);
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
		else {
			theResult.setError(Error::unknown_attribute, '\'' + theFieldName + '\'');
		}
	}

	// Validate all required fields are given.
	for (auto iter = m_attributes.cbegin(); theResult && iter != m_attributes.cend(); ++iter) {
		const auto& theAttribute = *iter;
		const std::string& theAttributeName = theAttribute.getName();
		if (!aRow.fieldExists(theAttributeName) && !theAttribute.isAutoIncr() && !theAttribute.isNullable()) {
			theResult.setError(Error::invalid_arguments, '\'' + theAttributeName + "' is NOT nullable, but value is not given");
			break;
		}
	}

	// Add fields that are not specified by user.
	for (auto iter = m_attributes.cbegin(); theResult && iter != m_attributes.cend(); ++iter) {
		const auto& theAttribute = *iter;
		const std::string& theAttributeName = iter->getName();
		if (!aRow.fieldExists(theAttributeName)) {
			URSQL_TRUTH(theAttribute.isAutoIncr() || theAttribute.isNullable(), "The attribute must be auto_increment or nullable!");
			if (theAttribute.isAutoIncr()) {
				aRow.addField(theAttributeName, Value(getNextAutoincr()));
			}
			else if (theAttribute.isNullable()) {
				aRow.addField(theAttributeName, theAttribute.getDefaultValue());
			}
		}
	}
	return theResult;
}

bool Entity::_blocknumExists(blocknum_t aBlocknum) const {
	return std::find(m_rowPos.cbegin(), m_rowPos.cend(), aBlocknum) != m_rowPos.cend();
}

size_type Entity::_indexOfBlocknum(blocknum_t aBlocknum) const {
	URSQL_TRUTH(_blocknumExists(aBlocknum), "Cannot find block number in Entity's row numbers");
	return std::distance(m_rowPos.cbegin(), std::find(m_rowPos.cbegin(), m_rowPos.cend(), aBlocknum));
}

}