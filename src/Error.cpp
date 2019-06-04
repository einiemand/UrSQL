#include "Error.hpp"
#include <unordered_map>

namespace UrSQL {

std::ostream& defaultOutput = std::cout;

StatusResult::StatusResult(Error code, std::string msg) :
	m_code(code),
	m_msg(std::move(msg))
{
}

StatusResult& StatusResult::operator=(StatusResult&& rhs) noexcept {
	if (!rhs) {
		m_msg = std::move(rhs.m_msg);
	}
	m_code = rhs.m_code;
	return *this;
}

void StatusResult::show_error(std::ostream& output) const {
	static std::unordered_map<Error, const char*> err2msg{

	};
	output << err2msg.at(m_code);
	if (!m_msg.empty()) {
		output << ": " << m_msg;
	}
	output << '\n';
}

}