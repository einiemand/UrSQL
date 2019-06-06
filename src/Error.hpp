#ifndef ERROR_HPP
#define ERROR_HPP
#include <iostream>
#include <xutility>

namespace UrSQL {

using size_type = std::size_t;

enum class Error {
	//parse related...
	keyword_expected = 100,
	identifier_expected = 110,
	unexpected_identifier = 115,
	key_expected = 120,
	value_expected = 130,
	unexpected_value = 135,
	syntax_error = 140,
	eof_error = 199,

	//statement related...
	statement_expected = 200,
	noDatabase_specified = 210,

	//table/DB related...
	table_exists = 300,
	database_exists,
	unknown_table = 310,
	unknown_database = 320,

	//type related...
	unknown_type = 400,
	unknown_attribute = 405,
	invalid_attribute = 410,
	invalid_arguments = 420,
	keyValue_mismatch = 430, //# of fieldnames doesn't match values...

	//storage/io related...
	read_error = 500,
	write_error = 510,
	seek_error = 520,
	storage_full = 530,

	//command related...
	unknown_command = 3000,
	invalid_command = 3010,

	//general purpose...
	user_terminated = 4998,
	not_implemented = 4999,
	no_error = 5000
};

extern std::ostream& defaultOutput;

class StatusResult {
public:

	StatusResult(Error aCode, std::string aMsg = "");
	~StatusResult() = default;

	StatusResult(StatusResult&& rhs) noexcept;
	StatusResult& operator=(StatusResult&& rhs) noexcept;

	inline operator bool() noexcept {
		return m_code == Error::no_error;
	}

	void set_error(Error aCode, std::string aMsg) noexcept;

	void show_error() const;

private:
	Error m_code;
	std::string m_msg;
};

inline void show_message(const char* what) {
	defaultOutput << what << '\n';
}


}

#endif /* Error.hpp */
