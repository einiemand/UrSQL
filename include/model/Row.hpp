#pragma once

#include <vector>

#include "Storable.hpp"
#include "Value.hpp"

namespace ursql {

class Row : public MonoStorable {
public:
    explicit Row(std::size_t blockNum);
    Row(std::size_t blockNum, std::vector<Value> values);
    ~Row() override = default;

    URSQL_DISABLE_COPY(Row);
    URSQL_DEFAULT_MOVE(Row);

    [[nodiscard]] BlockType expectedBlockType() const override;

    void serialize(BufferWriter& writer) const override;
    void deserialize(BufferReader& reader) override;

private:
    std::vector<Value> values_;
};

}  // namespace ursql
