#include "Storage.hpp"
#include <string>
#include <sstream>

namespace UrSQL {

const char* Storage::default_storage_path = "./tmp";
const char* Storage::default_file_extension = ".db";

Storage::Storage(std::string aFileName, CreateNewFile) :
	m_name(std::move(aFileName))
{
}

Storage::Storage(std::string aFileName, OpenExistingFile) :
	m_name(std::move(aFileName))
{
}

std::string Storage::get_file_path() const {
	std::ostringstream thePath;
	thePath << default_storage_path << '/' << m_name << default_file_extension;
	return thePath.str();
}

size_type Storage::get_block_count() {
	m_file.seekg(0, std::fstream::end);
	size_type total = static_cast<size_type>(m_file.tellg());
	return total / defaultBlockSize;
}

StatusResult Storage::get_free_block_number(blocknum_t& aFreeBlockNum) {
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
		aFreeBlockNum = get_block_count();
	}
	return theResult;
}

StatusResult Storage::setup_toc(const TOC& aTOC) {
	std::string theFilePath = get_file_path();

	m_file.open(theFilePath, std::fstream::trunc);
	m_file.close();

	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (storage_ready()) {
		Block theBlock(aTOC);
		return write_block(theBlock, aTOC.get_blocknum());
	}

	return StatusResult(Error::write_error, "file '" + m_name + "' cannot be opened");
}

StatusResult Storage::load_toc(TOC& aTOC) {
	std::string theFilePath = get_file_path();
	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (storage_ready()) {
		Block theBlock;

		StatusResult theResult = read_block(theBlock, 0);
		if (theResult) {
			aTOC.decode(theBlock, 0);
		}
		return theResult;
	}

	return StatusResult(Error::read_error, "file '" + m_name + "' cannot be opened");
}

StatusResult Storage::read_block(Block& aBlock, blocknum_t aBlockNum) {
	StatusResult theResult(Error::no_error);
	if (m_file.seekg(static_cast<int64_t>(aBlockNum) * defaultBlockSize)) {
		if (!m_file.read(reinterpret_cast<char*>(&aBlock), defaultBlockSize)) {
			theResult.set_error(Error::read_error, "unable to read blocks from '" + get_file_path() + '\'');
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
			theResult.set_error(Error::write_error, "unable to write blocks to '" + get_file_path() + '\'');
		}
	}
	else {
		theResult.set_error(Error::seek_error, "fstream offset error: " + std::to_string(aBlockNum));
	}
	return theResult;
}

StatusResult Storage::each_block(BlockVisitor aVisitor) {
	size_type theBlockCnt = get_block_count();

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


