#include "Error.hpp"
#include <unordered_map>

namespace UrSQL {

std::ostream& defaultOutput = std::cout;

StatusResult::StatusResult(Error code, std::string msg) :
	m_code(code),
	m_msg(std::move(msg))
{
}

StatusResult::StatusResult(StatusResult&& rhs) noexcept :
	m_code(rhs.m_code),
	m_msg(std::move(rhs.m_msg))
{
}

StatusResult& StatusResult::operator=(StatusResult&& rhs) noexcept {
	m_code = rhs.m_code;
	m_msg = std::move(rhs.m_msg);
	return *this;
}

void StatusResult::set_error(Error aCode, std::string aMsg) noexcept {
	m_code = aCode;
	m_msg = std::move(aMsg);
}

void StatusResult::show_error() const {
	static std::unordered_map<Error, const char*> err2msg{
		{ Error::syntax_error,"Syntax error" },
		{ Error::read_error,"Read error" },
		{ Error::write_error,"Write error" },
		{ Error::seek_error,"File seek error" },
		{ Error::block_invalidType,"Block type is invalid" },
		{ Error::block_fullData,"Block is full" },
		{ Error::block_notEnoughData,"Block didn't hold enough data to be parsed" },
		{ Error::folder_notExist,"Folder doesn't exist" },
		{ Error::unknown_command,"Unknown command" },
		{ Error::invalid_command,"Invalid command" },
		{ Error::not_implemented,"Functionality not implemented yet" }
	};
	defaultOutput << err2msg.at(m_code);
	if (!m_msg.empty()) {
		defaultOutput << ": " << m_msg;
	}
	defaultOutput << '\n';
}

}