#pragma once

#include <algorithm>
#include <vector>

#include "Attribute.hpp"

namespace ursql {

class Row;

class Entity : public MonoStorable {
public:
    explicit Entity(std::size_t blockNum);
    ~Entity() override = default;

    URSQL_DISABLE_COPY(Entity);
    URSQL_DEFAULT_MOVE(Entity);

    [[nodiscard]] BlockType expectedBlockType() const override;
    void serialize(BufferWriter& writer) const override;
    void deserialize(BufferReader& reader) override;

    void setAttributes(std::vector<Attribute> attributes);
    [[nodiscard]] const std::vector<Attribute>& getAttributes() const;

    std::size_t attributeIndex(std::string_view name) const;
    const Attribute& getAttribute(std::size_t index) const;

    std::size_t getNextAutoInc();

    void addRowPosition(std::size_t blockNum);
    void dropRowPosition(std::size_t blockNum);

    //    Row generateNewRow(const std::vector<std::string>& fieldNames, const
    //    StringList& aValueStrs);
    static constexpr const std::size_t npos =
      std::numeric_limits<std::size_t>::max();

private:
    std::vector<Attribute> attributes_;
    std::size_t autoInc_;
    std::vector<std::size_t> rowBlockNums_;
};

}  // namespace ursql
