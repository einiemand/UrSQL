#pragma once
#ifndef STORAGE_HPP
#define STORAGE_HPP
#include "common/Error.hpp"
#include <fstream>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>

#ifndef ENABLE_BLOCKCACHE
#define ENABLE_BLOCKCACHE
#endif

namespace UrSQL {

class Block;

#ifdef ENABLE_BLOCKCACHE
class BlockCache {
public:
	using UseSequence = std::list< std::pair<blocknum_t, std::unique_ptr<Block>> >;
 	using CacheMap = std::unordered_map<blocknum_t, UseSequence::iterator>;

	BlockCache(size_type aCapacity = BlockCache::capacity);
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

class Storage {
public:
	using BlockVisitor = std::function<StatusResult(Block&, blocknum_t)>;

	Storage(std::string aFileName, CreateNewFile, const TOC& aTOC, StatusResult& aResult);
	Storage(std::string aFileName, OpenExistingFile, TOC& aTOC, StatusResult& aResult);
	~Storage() = default;

	Storage(const Storage&) = delete;
	Storage& operator=(const Storage&) = delete;

	inline bool storageReady() const {
		return m_file.is_open();
	}

	inline const std::string& getName() const {
		return m_fileName;
	}

	StatusResult readBlock(Block& aBlock, blocknum_t aBlocknum);
	StatusResult writeBlock(const Block& aBlock, blocknum_t aBlocknum);
	StatusResult releaseBlock(blocknum_t aBlocknum);

	template<typename IterableT>
	StatusResult releaseBlocks(const IterableT& aBlocknumContainer) {
		StatusResult theResult(Error::no_error);
		for (auto iter = aBlocknumContainer.cbegin(); theResult && iter != aBlocknumContainer.cend(); ++iter) {
			theResult = releaseBlock(*iter);
		}
		return theResult;
	}

	StatusResult decodeMonoStorable(MonoStorable& aMonoStorable);
	StatusResult saveMonoStorable(const MonoStorable& aMonoStorable);

	StatusResult eachBlock(BlockVisitor aVisitor);

	template<typename IterableT>
	StatusResult visitBlocks(BlockVisitor aVisitor, const IterableT& anIterable) {
		StatusResult theResult(Error::no_error);

		Block theBlock;
		for (auto iter = anIterable.cbegin(); iter != anIterable.cend() && theResult; ++iter) {
			blocknum_t theBlocknum = *iter;
			theResult = readBlock(theBlock, theBlocknum);
			if (theResult) {
				theResult = aVisitor(theBlock, theBlocknum);
				if (theResult.getCode() == Error::block_found) {
					return theResult;
				}
			}
		}
		return theResult;
	}

	StatusResult findFreeBlocknumber(blocknum_t& aFreeBlocknum);

	static const char* defaultStoragePath;
	static const char* defaultFileExtension;
	static const size_type extensionLength;

	static std::string getDBFilePath(const std::string& aDBName);
	static bool hasDefaultExtension(const std::string& aFileName);
private:
	std::string m_fileName;
	std::fstream m_file;
#ifdef ENABLE_BLOCKCACHE
	BlockCache m_blockCache;
#endif

	blocknum_t _getBlockCount();

	StatusResult _setupTOC(const TOC& aTOC);
	StatusResult _loadTOC(TOC& aTOC);
};

}

#endif /* Storage.hpp */