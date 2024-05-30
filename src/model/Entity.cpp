#include "model/Entity.hpp"

#include <format>

#include "exception/InternalError.hpp"
#include "exception/UserError.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

Entity::Entity(std::size_t blockNum)
    : MonoStorable(blockNum),
      attributes_(),
      autoInc_(0),
      rowBlockNums_() {}

BlockType Entity::expectedBlockType() const {
    return BlockType::entity;
}

void Entity::serialize(BufferWriter& writer) const {
    writer << attributes_.size();
    for (auto& attribute : attributes_) {
        writer << attribute;
    }
    writer << autoInc_;
    writer << rowBlockNums_.size();
    for (std::size_t blockNum : rowBlockNums_) {
        writer << blockNum;
    }
}

void Entity::deserialize(BufferReader& reader) {
    for (auto attributeCount = reader.read<std::size_t>(); attributeCount > 0;
         --attributeCount)
    {
        attributes_.emplace_back(reader.read<Attribute>());
    }
    reader >> autoInc_;
    for (auto rowCount = reader.read<std::size_t>(); rowCount > 0; --rowCount) {
        rowBlockNums_.push_back(reader.read<std::size_t>());
    }
}

void Entity::setAttributes(std::vector<Attribute> attributes) {
    attributes_ = std::move(attributes);
    makeDirty(true);
}

const std::vector<Attribute>& Entity::getAttributes() const {
    return attributes_;
}

std::size_t Entity::attributeIndex(std::string_view name) const {
    auto it = std::ranges::find_if(attributes_, [name](auto& attribute) {
        return attribute.getName() == name;
    });
    URSQL_EXPECT(it != std::end(attributes_), DoesNotExist, name);
    return std::distance(std::begin(attributes_), it);
}

const Attribute& Entity::getAttribute(std::size_t index) const {
    URSQL_ASSERT(index < attributes_.size(), "attribute index out of range");
    return attributes_[index];
}

std::size_t Entity::getNextAutoInc() {
    makeDirty(true);
    return ++autoInc_;
}

void Entity::updateAutoInc(std::size_t i) {
    if (i > autoInc_) {
        autoInc_ = i;
        makeDirty(true);
    }
}

void Entity::addRowPosition(std::size_t blockNum) {
    auto it =
      std::find(std::begin(rowBlockNums_), std::end(rowBlockNums_), blockNum);
    URSQL_ASSERT(it == std::end(rowBlockNums_),
                 std::format("block num {} already exists", blockNum));
    rowBlockNums_.push_back(blockNum);
    makeDirty(true);
}

void Entity::dropRowPosition(std::size_t blockNum) {
    auto it =
      std::find(std::begin(rowBlockNums_), std::end(rowBlockNums_), blockNum);
    URSQL_ASSERT(it != std::end(rowBlockNums_),
                 std::format("block num {} doesn't exist", blockNum));
    rowBlockNums_.erase(it);
    makeDirty(true);
}

const std::vector<std::size_t>& Entity::getRowBlockNums() const {
    return rowBlockNums_;
}

// StatusResult Entity::generateNewRow(Row& aRow, const StringList& aFieldNames,
//                                     const StringList& aValueStrs) {
//     StatusResult theResult(Error::no_error);
//     for (size_type i = 0; theResult && i < aFieldNames.size(); ++i) {
//         const std::string& theFieldName = aFieldNames[i];
//         if (attributeExistsByName(theFieldName)) {
//             const Attribute& theAttribute = getAttributeByName(theFieldName);
//             const std::string& theValueStr = aValueStrs[i];
//             Value theValue(theValueStr);
//             theResult = theValue.become(theAttribute.getType());
//             if (theResult) {
//                 aRow.addField(theFieldName, std::move(theValue));
//             }
//         } else {
//             theResult.setError(Error::unknown_attribute,
//                                '\'' + theFieldName + '\'');
//         }
//     }
//
//     // Validate all required fields are given.
//     for (auto iter = m_attributes.cbegin();
//          theResult && iter != m_attributes.cend(); ++iter)
//     {
//         const auto& theAttribute = *iter;
//         const std::string& theAttributeName = theAttribute.getName();
//         if (!aRow.fieldExists(theAttributeName) && !theAttribute.isAutoIncr()
//         &&
//             !theAttribute.isNullable())
//         {
//             theResult.setError(Error::invalid_arguments,
//                                '\'' + theAttributeName +
//                                  "' is NOT nullable, but value is not
//                                  given");
//             break;
//         }
//     }
//
//     // Add fields that are not specified by user.
//     for (auto iter = m_attributes.cbegin();
//          theResult && iter != m_attributes.cend(); ++iter)
//     {
//         const auto& theAttribute = *iter;
//         const std::string& theAttributeName = iter->getName();
//         if (!aRow.fieldExists(theAttributeName)) {
//             URSQL_TRUTH(theAttribute.isAutoIncr() ||
//             theAttribute.isNullable(),
//                         "The attribute must be auto_increment or nullable!");
//             if (theAttribute.isAutoIncr()) {
//                 aRow.addField(theAttributeName, Value(getNextAutoincr()));
//             } else if (theAttribute.isNullable()) {
//                 aRow.addField(theAttributeName,
//                 theAttribute.getDefaultValue());
//             }
//         }
//     }
//     return theResult;
// }

}  // namespace ursql
