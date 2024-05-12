#include "model/Storable.hpp"

#include <format>
#include <iostream>

#include "common/Messaging.hpp"
#include "exception/InternalError.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

MonoStorable::MonoStorable(std::size_t blockNum)
    : Storable(),
      blockNum_(blockNum) {}

std::size_t MonoStorable::getBlockNum() const {
    return blockNum_;
}

void MonoStorable::encode(Block& block) const {
    block.setType(expectedBlockType());
    BufferWriter writer(block.getData(), Block::size);
    serialize(writer);
}

void MonoStorable::decode(const Block& block) {
    BlockType expected = expectedBlockType();
    BlockType actual = block.getType();
    URSQL_ASSERT(expected == actual, std::format("expected block type={}, actual={}", expected, actual));
    BufferReader reader(block.getData(), Block::size);
    deserialize(reader);
}

LazySaveMonoStorable::LazySaveMonoStorable(std::size_t blockNum)
    : MonoStorable(blockNum),
      dirty_(false) {}

bool LazySaveMonoStorable::isDirty() const {
    return dirty_;
}

void LazySaveMonoStorable::makeDirty(bool dirty) const {
    dirty_ = dirty;
}

}  // namespace ursql
