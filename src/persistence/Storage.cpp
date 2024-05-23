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

void Storage::readBlock(Block& block, std::size_t blockNum) {
    _read(&block, Block::size * blockNum, Block::size);
}

void Storage::writeBlock(const Block& block, std::size_t blockNum) {
    _write(&block, Block::size * blockNum, Block::size);
}

std::size_t Storage::getBlockCount() {
    file_.seekg(0, std::ios_base::end);
    std::size_t total = static_cast<std::size_t>(file_.tellg());
    return static_cast<std::size_t>(total / Block::size);
}

BlockType Storage::getBlockType(std::size_t blockNum) {
    BlockType blockType;
    _read(&blockType, Block::size * blockNum, sizeof(BlockType));
    return blockType;
}

void Storage::releaseBlock(std::size_t blockNum) {
    constexpr const BlockType freeType = BlockType::free;
    _write(&freeType, Block::size * blockNum, sizeof(BlockType));
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

void Storage::_read(void* dst, std::size_t offset, std::size_t len) {
    URSQL_EXPECT(file_.seekg(offset), FileAccessError, "seekg error");
    URSQL_EXPECT(file_.read(static_cast<char*>(dst), len), FileAccessError,
                 "read error");
}

void Storage::_write(const void* src, std::size_t offset, std::size_t len) {
    URSQL_EXPECT(file_.seekp(offset), FileAccessError, "seekp error");
    URSQL_EXPECT(file_.write(static_cast<const char*>(src), len),
                 FileAccessError, "write error");
}

}  // namespace ursql
