#include "Storage.hpp"
#include "TOC.hpp"
#include <sstream>

namespace UrSQL {

#ifdef ENABLE_BLOCKCACHE
/* -------------------------------BlockCache------------------------------- */
BlockCache::BlockCache(size_type aCapacity) :
	m_capacity(aCapacity),
	m_seq(),
	m_pos()
{
}

bool BlockCache::contains(blocknum_t aBlocknum) {
	return m_pos.count(aBlocknum) == 1;
}

void BlockCache::put(blocknum_t aBlocknum, const Block& aBlock) {
	if (contains(aBlocknum)) {
		_touch(aBlocknum)->second = std::make_unique<Block>(aBlock);
	}
	else {
		_add(aBlocknum, aBlock);
	}
}

const Block& BlockCache::get(blocknum_t aBlocknum) {
	if (!contains(aBlocknum)) {
		throw std::runtime_error("Impossible: getting a block that's not in cache");
	}
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
	m_seq.push_front({ aBlocknum,std::make_unique<Block>(aBlock) });
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
const char* Storage::defaultStoragePath = "./tmp";
const char* Storage::defaultFileExtension = ".db";
const size_type Storage::extensionLength = strlen(Storage::defaultFileExtension);

Storage::Storage(std::string aFileName, CreateNewFile, const TOC& aTOC, StatusResult& aResult) :
	m_fileName(std::move(aFileName))
{
	aResult = _setupTOC(aTOC);
}

Storage::Storage(std::string aFileName, OpenExistingFile, TOC& aTOC, StatusResult& aResult) :
	m_fileName(std::move(aFileName))
{
	aResult = _loadTOC(aTOC);
}

std::string Storage::getDBFilePath(const std::string& aDBName) {
	std::ostringstream thePath;
	thePath << defaultStoragePath << '/' << aDBName << defaultFileExtension;
	return thePath.str();
}

bool Storage::hasDefaultExtension(const std::string& aFileName) {
	return aFileName.substr(aFileName.length() - Storage::extensionLength) == Storage::defaultFileExtension;
}

blocknum_t Storage::_getBlockCount() {
	m_file.seekg(0, std::fstream::end);
	size_type total = static_cast<size_type>(m_file.tellg());
	return static_cast<blocknum_t>(total / defaultBlockSize);
}

StatusResult Storage::_setupTOC(const TOC& aTOC) {
	std::string theFilePath = Storage::getDBFilePath(m_fileName);
	//m_file.clear();

	m_file.open(theFilePath, std::fstream::out | std::fstream::trunc);
	m_file.close();

	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (storageReady()) {
		return saveMonoStorable(aTOC);
	}

	return StatusResult(Error::write_error, "File '" + theFilePath + "' cannot be opened");
}

StatusResult Storage::_loadTOC(TOC& aTOC) {
	std::string theFilePath = Storage::getDBFilePath(m_fileName);
	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (storageReady()) {
		Block theBlock;

		StatusResult theResult = readBlock(theBlock, 0);
		if (theResult) {
			aTOC.decode(theBlock);
		}
		return theResult;
	}

	return StatusResult(Error::read_error, "File '" + m_fileName + "' cannot be opened");
}

StatusResult Storage::readBlock(Block& aBlock, blocknum_t aBlocknum) {
#ifdef ENABLE_BLOCKCACHE
	if (m_blockCache.contains(aBlocknum)) {
		aBlock = m_blockCache.get(aBlocknum);
		return StatusResult(Error::no_error);
	}
#endif
	StatusResult theResult(Error::no_error);
	if (m_file.seekg(static_cast<int64_t>(aBlocknum) * defaultBlockSize)) {
		if (!m_file.read(reinterpret_cast<char*>(&aBlock), defaultBlockSize)) {
			theResult.setError(Error::read_error, "Unable to read blocks from '" + Storage::getDBFilePath(m_fileName) + '\'');
		}
#ifdef ENABLE_BLOCKCACHE
		else {
			m_blockCache.put(aBlocknum, aBlock);
		}
#endif
	}
	else {
		theResult.setError(Error::seek_error, "fstream offset error: " + std::to_string(aBlocknum));
	}
	return theResult;
}

StatusResult Storage::writeBlock(const Block& aBlock, blocknum_t aBlocknum) {
	StatusResult theResult(Error::no_error);
	if (m_file.seekp(static_cast<int64_t>(aBlocknum) * defaultBlockSize)) {
		if (!m_file.write(reinterpret_cast<const char*>(&aBlock), defaultBlockSize)) {
			theResult.setError(Error::write_error, "Unable to write blocks to '" + Storage::getDBFilePath(m_fileName) + '\'');
		}
#ifdef ENABLE_BLOCKCACHE
		else {
			m_blockCache.put(aBlocknum, aBlock);
		}
#endif
	}
	else {
		theResult.setError(Error::seek_error, "fstream offset error: " + std::to_string(aBlocknum));
	}
	return theResult;
}

StatusResult Storage::releaseBlock(blocknum_t aBlocknum) {
	Block theBlock(BlockType::free_type);
	return writeBlock(theBlock, aBlocknum);
}

StatusResult Storage::decodeMonoStorable(MonoStorable& aMonoStorable) {
	Block theBlock;
	StatusResult theResult = readBlock(theBlock, aMonoStorable.getBlocknum());
	if (theResult) {
		aMonoStorable.decode(theBlock);
	}
	return theResult;
}

StatusResult Storage::saveMonoStorable(const MonoStorable& aMonoStorable) {
	return writeBlock(Block(aMonoStorable), aMonoStorable.getBlocknum());
}

StatusResult Storage::eachBlock(BlockVisitor aVisitor) {
	blocknum_t theBlockCnt = _getBlockCount();

	StatusResult theResult(Error::no_error);

	Block theBlock;
	for (blocknum_t theBlocknum = 0; theResult && theBlocknum < theBlockCnt; ++theBlocknum) {
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

StatusResult Storage::findFreeBlocknumber(blocknum_t& aFreeBlocknum) {
	StatusResult theResult = eachBlock(
		[&aFreeBlocknum](Block& aBlock, blocknum_t aBlocknum)->StatusResult {
			if (aBlock.getType() == BlockType::free_type) {
				aFreeBlocknum = aBlocknum;
				return StatusResult(Error::block_found);
			}
			return StatusResult(Error::no_error);
		}
	);
	if (theResult) {
		aFreeBlocknum = _getBlockCount();
	}
	if (theResult.getCode() == Error::block_found) {
		theResult.setError(Error::no_error);
	}
	return theResult;
}

} /* UrSQL */


