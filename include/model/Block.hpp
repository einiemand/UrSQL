#pragma once

#include <unordered_map>

#include "common/Util.hpp"

namespace UrSQL {

enum class BlockType : char {
    TOC_type = 'C',
    entity_type = 'E',
    index_type = 'I',
    row_type = 'R',
    free_type = 'F'
};

constexpr size_type defaultBlockSize = 1024;
constexpr size_type defaultPayloadSize = defaultBlockSize - sizeof(BlockType);

class MonoStorable;

class Block {
public:
    explicit Block(BlockType aType = BlockType::free_type);
    explicit Block(const MonoStorable& aMonoStorable);
    ~Block() = default;

    Block(const Block& rhs) noexcept;
    Block& operator=(const Block& rhs) noexcept;

    inline BlockType getType() const {
        return m_type;
    }

    inline void setType(BlockType aType) {
        m_type = aType;
    }

    inline const char* getData() const {
        return m_Data;
    }

    inline char* getData() {
        return m_Data;
    }

private:
    BlockType m_type;
    char m_Data[defaultPayloadSize];
};

static_assert(sizeof(Block) == defaultBlockSize,
              "Block size is not equal to defaultBlockSize. Fix it!");

}  // namespace UrSQL
