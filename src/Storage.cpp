#include "Storage.hpp"
#include "TOC.hpp"
#include <string>
#include <sstream>

namespace UrSQL {

const char* Storage::default_storage_path = "./tmp";
const char* Storage::default_file_extension = ".db";

Storage::Storage(std::string aFileName, CreateNewFile, const TOC& aTOC, StatusResult& aResult) :
	m_name(std::move(aFileName))
{
	aResult = _setup_toc(aTOC);
}

Storage::Storage(std::string aFileName, OpenExistingFile, TOC& aTOC, StatusResult& aResult) :
	m_name(std::move(aFileName))
{
	aResult = _load_toc(aTOC);
}

std::string Storage::get_dbfile_path(const std::string& aDBName) {
	std::ostringstream thePath;
	thePath << default_storage_path << '/' << aDBName << default_file_extension;
	return thePath.str();
}

size_type Storage::_get_block_count() {
	m_file.seekg(0, std::fstream::end);
	size_type total = static_cast<size_type>(m_file.tellg());
	return total / defaultBlockSize;
}

StatusResult Storage::_get_free_block_number(blocknum_t& aFreeBlockNum) {
	StatusResult theResult = each_block(
		[&aFreeBlockNum](Block& aBlock, blocknum_t aBlockNum)->StatusResult {
			if (aBlock.get_type() == BlockType::free_type) {
				aFreeBlockNum = aBlockNum;
				return StatusResult(Error::block_found);
			}
			return StatusResult(Error::no_error);
		}
	);
	if (theResult.get_code() == Error::block_found) {
		return StatusResult(Error::no_error);
	}
	if (theResult) {
		aFreeBlockNum = _get_block_count();
	}
	return theResult;
}

StatusResult Storage::_setup_toc(const TOC& aTOC) {
	std::string theFilePath = Storage::get_dbfile_path(m_name);
	//m_file.clear();

	m_file.open(theFilePath, std::fstream::out | std::fstream::trunc);
	m_file.close();

	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (storage_ready()) {
		Block theBlock(aTOC);
		return write_block(theBlock, aTOC.get_blocknum());
	}

	return StatusResult(Error::write_error, "File '" + theFilePath + "' cannot be opened");
}

StatusResult Storage::_load_toc(TOC& aTOC) {
	std::string theFilePath = Storage::get_dbfile_path(m_name);
	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (storage_ready()) {
		Block theBlock;

		StatusResult theResult = read_block(theBlock, 0);
		if (theResult) {
			aTOC.decode(theBlock, 0);
		}
		return theResult;
	}

	return StatusResult(Error::read_error, "File '" + m_name + "' cannot be opened");
}

StatusResult Storage::read_block(Block& aBlock, blocknum_t aBlockNum) {
	StatusResult theResult(Error::no_error);
	if (m_file.seekg(static_cast<int64_t>(aBlockNum) * defaultBlockSize)) {
		if (!m_file.read(reinterpret_cast<char*>(&aBlock), defaultBlockSize)) {
			theResult.set_error(Error::read_error, "Unable to read blocks from '" + Storage::get_dbfile_path(m_name) + '\'');
		}
	}
	else {
		theResult.set_error(Error::seek_error, "fstream offset error: " + std::to_string(aBlockNum));
	}
	return theResult;
}

StatusResult Storage::write_block(const Block& aBlock, blocknum_t aBlockNum) {
	StatusResult theResult(Error::no_error);
	if (m_file.seekp(static_cast<int64_t>(aBlockNum) * defaultBlockSize)) {
		if (!m_file.write(reinterpret_cast<const char*>(&aBlock), defaultBlockSize)) {
			theResult.set_error(Error::write_error, "Unable to write blocks to '" + Storage::get_dbfile_path(m_name) + '\'');
		}
	}
	else {
		theResult.set_error(Error::seek_error, "fstream offset error: " + std::to_string(aBlockNum));
	}
	return theResult;
}

StatusResult Storage::each_block(BlockVisitor aVisitor) {
	size_type theBlockCnt = _get_block_count();

	StatusResult theResult(Error::no_error);

	Block theBlock;
	for (size_type theBlockNum = 0; theResult && theBlockNum < theBlockCnt; ++theBlockNum) {
		theResult = read_block(theBlock, theBlockNum);
		if (theResult) {
			theResult = aVisitor(theBlock, theBlockNum);
		}
	}
	return theResult;
}

} /* UrSQL */


