#pragma once

#include <algorithm>

#include "Attribute.hpp"

namespace ursql {

class Row;

class Entity : public LazySaveMonoStorable {
public:
    explicit Entity(std::size_t blockNum);
    ~Entity() override = default;

    URSQL_DISABLE_COPY(Entity);

    [[nodiscard]] BlockType expectedBlockType() const override;
    void serialize(BufferWriter& writer) const override;
    void deserialize(BufferReader& reader) override;

    void addAttribute(Attribute&& attribute);

    bool attributeExistsByName(std::string_view name) const;
    const Attribute& getAttributeByName(std::string_view name) const;

    const std::vector<Attribute>& getAttributes() const;

    std::size_t getNextAutoInc();

    void addRowPosition(std::size_t blockNum);
    void dropRowPosition(std::size_t blockNum);

    const std::vector<std::size_t>& getRowBlockNums();

    //    Row generateNewRow(const std::vector<std::string>& fieldNames, const
    //    StringList& aValueStrs);

private:
    std::vector<Attribute> attributes_;
    std::size_t autoInc_;
    std::vector<std::size_t> rowBlockNums_;
};

}  // namespace ursql
