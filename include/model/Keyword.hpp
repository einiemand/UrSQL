#pragma once

#include <unordered_map>

namespace UrSQL {

enum class Keyword {
	add_kw, and_kw, asc_kw, auto_increment_kw, boolean_kw,
	by_kw, char_kw, create_kw, database_kw, databases_kw, desc_kw,
	default_kw, delete_kw, describe_kw, double_kw, drop_kw, float_kw,
	from_kw, group_kw, help_kw, in_kw, insert_kw,
	integer_kw, into_kw, key_kw, not_kw, null_kw, or_kw,
	order_kw, primary_kw, quit_kw, select_kw, set_kw,
	show_kw, table_kw, tables_kw, unique_kw, unknown_kw,
	update_kw, use_kw, values_kw, varchar_kw, version_kw,
	where_kw
};

bool isKeyword(const std::string& aWord);

Keyword toKeyword(const std::string& aWord);

}
