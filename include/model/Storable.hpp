#pragma once

#include "common/Error.hpp"
#include "persistence/Block.hpp"

namespace ursql {

class BufferWriter;
class BufferReader;

class Storable {
public:
    explicit Storable() = default;
    virtual ~Storable() = default;

    virtual void serialize(BufferWriter& writer) const = 0;
    virtual void deserialize(BufferReader& reader) = 0;
};

class MonoStorable : public Storable {
public:
    explicit MonoStorable(std::size_t blockNum);
    ~MonoStorable() override = default;

    [[nodiscard]] std::size_t getBlockNum() const;
    [[nodiscard]] virtual BlockType expectedBlockType() const = 0;

    void encode(Block& block) const;
    void decode(const Block& block);

protected:
    std::size_t blockNum_;
};

class LazySaveMonoStorable : public MonoStorable {
public:
    explicit LazySaveMonoStorable(std::size_t blockNum);
    ~LazySaveMonoStorable() override = default;

    [[nodiscard]] bool isDirty() const;
    void makeDirty(bool dirty) const;

private:
    mutable bool dirty_;
};

}  // namespace ursql
