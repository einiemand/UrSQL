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

blocknum_t Storage::get_free_block_number() {
	return m_set.empty() ? get_block_count() : *m_set.cbegin();
}

StatusResult Storage::setup_toc(const TOC& aTOC) {
	std::string theFilePath = get_file_path();

	m_file.open(theFilePath, std::fstream::out | std::fstream::trunc | std::fstream::binary);
	m_file.close();

	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (ready()) {
		Block theBlock(aTOC);
		return write_block(theBlock, 0);
	}

	return StatusResult(Error::write_error, "file " + m_name + " cannot be opened");
}

StatusResult Storage::load_toc(TOC& aTOC) {
	std::string theFilePath = get_file_path();
	m_file.open(theFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);

	if (ready()) {
		Block theBlock;

		StatusResult theResult = read_block(theBlock, 0);
		if (theResult) {
			theResult = aTOC.decode(theBlock);
		}
		return theResult;
	}

	return StatusResult(Error::read_error, "file " + m_name + " cannot be opened");
}

StatusResult Storage::read_block(Block& aBlock, blocknum_t aBlockNum) {
	StatusResult theResult(Error::no_error);
	if (m_file.seekg(static_cast<int64_t>(aBlockNum) * defaultBlockSize)) {
		if (!m_file.read(reinterpret_cast<char*>(&aBlock), defaultBlockSize)) {
			theResult.set_error(Error::read_error, "unable to read blocks from " + get_file_path());
		}
	}
	else {
		theResult.set_error(Error::read_error, "fstream offset error: " + std::to_string(aBlockNum));
	}
	return theResult;
}

StatusResult Storage::write_block(const Block& aBlock, blocknum_t aBlockNum) {
	StatusResult theResult(Error::no_error);
	if (m_file.seekp(static_cast<int64_t>(aBlockNum) * defaultBlockSize)) {
		if (!m_file.write(reinterpret_cast<const char*>(&aBlock), defaultBlockSize)) {
			theResult.set_error(Error::read_error, "unable to write blocks to " + get_file_path());
		}
	}
	else {
		theResult.set_error(Error::read_error, "fstream offset error: " + std::to_string(aBlockNum));
	}
	return theResult;
}

} /* UrSQL */


