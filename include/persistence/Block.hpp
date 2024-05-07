#pragma once

#include <unordered_map>

#include "common/Macros.hpp"

namespace ursql {

enum class BlockType : char {
    toc = 'C',
    entity = 'E',
    index = 'I',
    row = 'R',
    free = 'F'
};

class MonoStorable;

class Block {
public:
    explicit Block(BlockType aType = BlockType::free);
    explicit Block(const MonoStorable& aMonoStorable);
    ~Block() = default;

    URSQL_DISABLE_COPY(Block);

    [[nodiscard]] BlockType getType() const;

    void setType(BlockType type);

    [[nodiscard]] const char* getData() const;

    [[nodiscard]] char* getData();

    static constexpr const std::size_t size = 1024;
    static constexpr const std::size_t payloadSize = size - sizeof(BlockType);

private:
    BlockType type_;

    union {
        char nullState_;
        char data_[payloadSize];
    };
};

static_assert(sizeof(Block) == Block::size, "Block size is not 1024");

}  // namespace ursql
