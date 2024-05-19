#pragma once

#include <fstream>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#include "common/Macros.hpp"

#ifndef ENABLE_BLOCKCACHE
#endif

namespace ursql {

class Block;

#ifdef ENABLE_BLOCKCACHE
class BlockCache {
public:
    using UseSequence =
      std::list<std::pair<blocknum_t, std::unique_ptr<Block>>>;
    using CacheMap = std::unordered_map<blocknum_t, UseSequence::iterator>;

    explicit BlockCache(size_type aCapacity = BlockCache::capacity);
    ~BlockCache() = default;

    URSQL_DISABLE_COPY(BlockCache);

    bool contains(blocknum_t aBlocknum);
    void put(blocknum_t aBlocknum, const Block& aBlock);
    const Block& get(blocknum_t aBlocknum);

    static constexpr size_type capacity = 50;

private:
    const size_type m_capacity;
    UseSequence m_seq;
    CacheMap m_pos;

    UseSequence::iterator _touch(blocknum_t aBlocknum);
    void _add(blocknum_t aBlocknum, const Block& aBlock);
    void _adjustSize();
    void _removeTail();
};
#endif

struct OpenExistingFile {};

struct CreateNewFile {};

class TOC;
class MonoStorable;
class LazySaveMonoStorable;

namespace fs = std::filesystem;

class Storage {
public:
    using BlockVisitor = std::function<bool(Block&, std::size_t)>;

    Storage(const fs::path& filePath, CreateNewFile);
    Storage(const fs::path& filePath, OpenExistingFile);

    ~Storage() = default;

    URSQL_DISABLE_COPY(Storage);
    URSQL_DEFAULT_MOVE(Storage);

    void readBlock(Block& block, std::size_t blockNum);
    void writeBlock(const Block& block, std::size_t blockNum);
    void releaseBlock(std::size_t blockNum);

    std::size_t findFreeBlockNumber();

    void save(const MonoStorable& monoStorable);
    void saveIfDirty(const LazySaveMonoStorable& lazySaveMonoStorable);
    void load(MonoStorable& monoStorable);
private:
    std::fstream file_;
#ifdef ENABLE_BLOCKCACHE
    BlockCache m_blockCache;
#endif
    std::size_t _getBlockCount();
};

}  // namespace ursql
