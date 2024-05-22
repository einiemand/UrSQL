#include "persistence/Storage.hpp"

#include <format>
#include <sstream>

#include "exception/InternalError.hpp"
#include "model/TOC.hpp"

namespace ursql {

#ifdef ENABLE_BLOCKCACHE
/* -------------------------------BlockCache------------------------------- */
BlockCache::BlockCache(size_type aCapacity)
    : m_capacity(aCapacity),
      m_seq(),
      m_pos() {}

bool BlockCache::contains(blocknum_t aBlocknum) {
    return m_pos.count(aBlocknum) == 1;
}

void BlockCache::put(blocknum_t aBlocknum, const Block& aBlock) {
    if (contains(aBlocknum)) {
        _touch(aBlocknum)->second = std::make_unique<Block>(aBlock);
    } else {
        _add(aBlocknum, aBlock);
    }
}

const Block& BlockCache::get(blocknum_t aBlocknum) {
    URSQL_TRUTH(contains(aBlocknum), "Getting a block that's not in cache");
    return *(_touch(aBlocknum)->second);
}

BlockCache::UseSequence::iterator BlockCache::_touch(blocknum_t aBlocknum) {
    if (contains(aBlocknum)) {
        auto theIter = m_pos[aBlocknum];
        m_seq.push_front(std::move(*theIter));
        m_seq.erase(theIter);
        m_pos[aBlocknum] = m_seq.begin();
    }
    return m_seq.begin();
}

void BlockCache::_add(blocknum_t aBlocknum, const Block& aBlock) {
    m_seq.push_front({ aBlocknum, std::make_unique<Block>(aBlock) });
    m_pos[aBlocknum] = m_seq.begin();
    _adjustSize();
}

void BlockCache::_adjustSize() {
    if (m_seq.size() > m_capacity) {
        _removeTail();
    }
}

void BlockCache::_removeTail() {
    blocknum_t theBlocknum = m_seq.back().first;
    m_seq.pop_back();
    m_pos.erase(theBlocknum);
}
#endif

/* -------------------------------Storage------------------------------- */
Storage::Storage(const fs::path& filePath, CreateNewFile)
    : file_(filePath, std::ios_base::binary | std::ios_base::in |
                        std::ios_base::out | std::ios_base::trunc) {
    URSQL_EXPECT(file_, FileAccessError,
                 std::format("unable to create file {}", filePath.native()));
}

Storage::Storage(const fs::path& filePath, OpenExistingFile)
    : file_(filePath,
            std::ios_base::binary | std::ios_base::in | std::ios_base::out) {
    URSQL_EXPECT(file_, FileAccessError,
                 std::format("unable to open file {}", filePath.native()));
}

std::size_t Storage::_getBlockCount() {
    file_.seekg(0, std::ios_base::end);
    std::size_t total = static_cast<std::size_t>(file_.tellg());
    return static_cast<std::size_t>(total / Block::size);
}

void Storage::readBlock(Block& block, std::size_t blockNum) {
#ifdef ENABLE_BLOCKCACHE
    if (m_blockCache.contains(aBlocknum)) {
        aBlock = m_blockCache.get(aBlocknum);
        return StatusResult(Error::no_error);
    }
#endif
    if (file_.seekg(blockNum * Block::size)) {
        if (!file_.read(reinterpret_cast<char*>(&block), Block::size)) {
            URSQL_THROW_TRACED(FileAccessError, "unable to read from file");
        }
#ifdef ENABLE_BLOCKCACHE
        else
        {
            m_blockCache.put(aBlocknum, aBlock);
        }
#endif
    } else {
        URSQL_THROW_TRACED(FileAccessError,
                           "unable to read from file. Probably offset issue");
    }
}

void Storage::writeBlock(const Block& block, std::size_t blockNum) {
    if (file_.seekp(blockNum * Block::size)) {
        if (!file_.write(reinterpret_cast<const char*>(&block), Block::size)) {
            URSQL_THROW_TRACED(FileAccessError, "unable to write to file");
        }
#ifdef ENABLE_BLOCKCACHE
        else
        {
            m_blockCache.put(aBlocknum, aBlock);
        }
#endif
    } else {
        URSQL_THROW_TRACED(FileAccessError,
                           "unable to write to file. Probably offset issue");
    }
}

void Storage::releaseBlock(std::size_t blockNum) {
    Block theBlock(BlockType::free);
    writeBlock(theBlock, blockNum);
}

void Storage::save(const MonoStorable& monoStorable) {
    if (monoStorable.isDirty()) {
        Block block;
        monoStorable.encode(block);
        writeBlock(block, monoStorable.getBlockNum());
        monoStorable.makeDirty(false);
    }
}

void Storage::load(MonoStorable& monoStorable) {
    Block block;
    readBlock(block, monoStorable.getBlockNum());
    monoStorable.decode(block);
    monoStorable.makeDirty(false);
}

std::size_t Storage::findFreeBlockNumber() {
    std::size_t blockCnt = _getBlockCount();
    for (std::size_t i = 0; i < blockCnt; ++i) {
        Block block;
        readBlock(block, i);
        if (block.getType() == BlockType::free) {
            return i;
        }
    }
    return blockCnt;
}

}  // namespace ursql
