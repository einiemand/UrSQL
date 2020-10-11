#pragma once

#include "Block.hpp"
#include "common/Error.hpp"

namespace UrSQL {

class BufferWriter;
class BufferReader;

class Storable {
public:
    Storable() = default;
    virtual ~Storable() = default;

    virtual void serialize(BufferWriter& aWriter) const = 0;
    virtual void deserialize(BufferReader& aReader) = 0;
};

class MonoStorable : public Storable {
public:
    explicit MonoStorable(blocknum_t aBlocknum);
    ~MonoStorable() override = default;

    virtual BlockType expectedBlockType() const = 0;

    void encode(Block& aBlock) const;
    void decode(const Block& aBlock);

    inline blocknum_t getBlocknum() const {
        return m_blocknum;
    }

    inline void setBlocknum(blocknum_t aBlocknum) {
        m_blocknum = aBlocknum;
    }

protected:
    blocknum_t m_blocknum;
};

}  // namespace UrSQL
