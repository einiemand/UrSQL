#pragma once
#ifndef STORAGE_HPP
#define STORAGE_HPP
#include "Error.hpp"
#include <fstream>
#include <set>
#include <functional>

namespace UrSQL {

struct OpenExistingFile {};
struct CreateNewFile {};

class Block;
class TOC;
class MonoStorable;

class Storage {
public:
	using BlockVisitor = std::function<StatusResult(Block&, blocknum_t)>;

	template<typename RawT>
	using BlocknumExtractor = std::function<blocknum_t(const RawT&)>;

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

	template<typename IterableT, typename RawT>
	StatusResult releaseBlocks(const IterableT& aContainer, BlocknumExtractor<RawT> anExtractor) {
		StatusResult theResult(Error::no_error);
		for (auto iter = aContainer.cbegin(); theResult && iter != aContainer.cend(); ++iter) {
			blocknum_t theBlocknum = anExtractor(*iter);
			theResult = releaseBlock(theBlocknum);
		}
		return theResult;
	}

	StatusResult decodeMonoStorable(MonoStorable& aMonoStorable);

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

	blocknum_t _getBlockCount();

	StatusResult _setupTOC(const TOC& aTOC);
	StatusResult _loadTOC(TOC& aTOC);
};

}

#endif /* Storage.hpp */