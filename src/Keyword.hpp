#pragma once
#ifndef KEYWORD_HPP
#define KEYWORD_HPP
#include <unordered_map>

namespace UrSQL {

enum class Keyword {
	add_kw, and_kw, auto_increment_kw, boolean_kw,
	by_kw, char_kw, create_kw, database_kw, databases_kw,
	delete_kw, describe_kw, double_kw, drop_kw, float_kw,
	from_kw, group_kw, help_kw, in_kw, insert_kw,
	integer_kw, into_kw, key_kw, not_kw, null_kw, or_kw,
	order_kw, primary_kw, quit_kw, select_kw, set_kw,
	show_kw, table_kw, tables_kw, unique_kw, unknown_kw,
	update_kw, use_kw, values_kw, varchar_kw, version_kw,
	where_kw
};

bool is_keyword(const char* aWord);

Keyword to_keyword(const char* aWord);

inline bool is_keyword(const std::string& aWord) {
	return is_keyword(aWord.c_str());
}

inline Keyword to_keyword(const std::string& aWord) {
	return to_keyword(aWord.c_str());
}

}
#endif /* Keyword.hpp */