#include "persistence/Block.hpp"

#include "model/Storable.hpp"
#include "persistence/BufferStream.hpp"

namespace ursql {

Block::Block(BlockType type) : type_(type), nullState_{} {}

Block::Block(const MonoStorable& aMonoStorable) : Block(BlockType::free) {
    aMonoStorable.encode(*this);
}

BlockType Block::getType() const {
    return type_;
}

void Block::setType(BlockType type) {
    type_ = type;
}

const char* Block::getData() const {
    return data_;
}

char* Block::getData() {
    return data_;
}

}  // namespace ursql
