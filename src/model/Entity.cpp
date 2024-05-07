#include "model/Entity.hpp"

#include <format>

#include "exception/InternalError.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

Entity::Entity(std::size_t blockNum)
    : LazySaveMonoStorable(blockNum),
      attributes_(),
      autoInc_(1),
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

void Entity::addAttribute(Attribute&& attribute) {
    attributes_.emplace_back(std::move(attribute));
    makeDirty(true);
}

bool Entity::attributeExistsByName(std::string_view name) const {
    return std::find_if(std::begin(attributes_), std::end(attributes_),
                        [&name](auto& attribute) -> bool {
                            return attribute.getName() == name;
                        }) != std::end(attributes_);
}

const Attribute& Entity::getAttributeByName(std::string_view name) const {
    auto it = std::find_if(std::begin(attributes_), std::end(attributes_),
                           [&name](auto& attribute) {
                               return attribute.getName() == name;
                           });
    URSQL_ASSERT(it != std::end(attributes_),
                 std::format("attribute {} doesn't exist", name));
    return *it;
}

std::size_t Entity::getNextAutoInc() {
    makeDirty(true);
    return autoInc_++;
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
