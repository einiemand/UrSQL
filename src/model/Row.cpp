#include "model/Row.hpp"

#include "persistence/BufferStream.hpp"

namespace ursql {

Row::Row(std::size_t blockNum) : MonoStorable(blockNum) {}

Row::Row(std::size_t blockNum, std::vector<Value> values)
    : MonoStorable(blockNum),
      values_(std::move(values)) {}

BlockType Row::expectedBlockType() const {
    return BlockType::row;
}

void Row::serialize(BufferWriter& writer) const {
    writer << values_.size();
    for (auto& value : values_) {
        writer << value;
    }
}

void Row::deserialize(BufferReader& reader) {
    auto sz = reader.read<std::size_t>();
    for (std::size_t i = 0; i < sz; ++i) {
        values_[i] = reader.read<Value>();
    }
}

}  // namespace ursql
