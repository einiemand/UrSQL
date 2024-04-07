#include "common/Error.hpp"

#include <unordered_map>

namespace UrSQL {

std::ostream& defaultOutput = std::cout;

StatusResult::StatusResult(Error code, std::string msg)
    : m_code(code),
      m_msg(std::move(msg)) {}

StatusResult::StatusResult(StatusResult&& rhs) noexcept
    : m_code(rhs.m_code),
      m_msg(std::move(rhs.m_msg)) {}

StatusResult& StatusResult::operator=(StatusResult&& rhs) noexcept {
    m_code = rhs.m_code;
    m_msg = std::move(rhs.m_msg);
    return *this;
}

void StatusResult::setError(Error aCode, std::string aMsg) noexcept {
    m_code = aCode;
    m_msg = std::move(aMsg);
}

void StatusResult::showError() const {
    static std::unordered_map<Error, const char*> err2msg{
        { Error::unexpected_identifier, "Unexpected identifier" },
        { Error::syntax_error, "Syntax error" },
        { Error::database_exists, "Database alreay exists" },
        { Error::unknown_database, "Unknown database" },
        { Error::read_error, "Read error" },
        { Error::write_error, "Write error" },
        { Error::seek_error, "File seek error" },
        { Error::delete_error, "File deletion error" },
        { Error::block_invalidType, "Block type is invalid" },
        { Error::block_fullData, "Block is full" },
        { Error::block_notEnoughData,
          "Block didn't hold enough data to be parsed" },
        { Error::folder_notExist, "Folder doesn't exist" },
        { Error::unknown_command, "Unknown command" },
        { Error::invalid_command, "Invalid command" },
        { Error::not_implemented, "Functionality not implemented yet" }
    };
    if (m_code != Error::no_error && m_code != Error::user_terminated) {
        auto iter = err2msg.find(m_code);
        defaultOutput << (iter != err2msg.end() ? iter->second :
                                                  "Unsupported error type")
                      << ": ";
    }
    defaultOutput << m_msg;
}

}  // namespace UrSQL
