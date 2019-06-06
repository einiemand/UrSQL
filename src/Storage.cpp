#include "Storage.hpp"
#include <string>
#include <sstream>

namespace UrSQL {

const char* Storage::default_storage_path = "./tmp";
const char* Storage::default_file_extension = ".db";

Storage::Storage(std::string aFileName, OpenExistingFile) :
	m_name(std::move(aFileName))
{
}

Storage::Storage(std::string aFileName, CreateNewFile) :
	m_name(std::move(aFileName))
{

}

std::string Storage::get_file_path() const {
	std::ostringstream thePath;
	thePath << default_storage_path << '/' << m_name << default_file_extension;
	return thePath.str();
}

StatusResult Storage::read_block(Block& aBlock, int32_t aBlockNum) {
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

StatusResult Storage::write_block(const Block& aBlock, int32_t aBlockNum) {
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


