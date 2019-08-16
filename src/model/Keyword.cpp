#include "model/Keyword.hpp"

namespace UrSQL {

const std::unordered_map<std::string, Keyword> str2keyword{
	{ "add",       Keyword::add_kw },
	{ "and",       Keyword::and_kw },
	{ "asc",       Keyword::asc_kw },
	{ "auto_increment", Keyword::auto_increment_kw },
	{ "boolean",   Keyword::boolean_kw },
	{ "by",        Keyword::by_kw },
	{ "char",      Keyword::char_kw },
	{ "create",    Keyword::create_kw },
	{ "database",  Keyword::database_kw },
	{ "databases", Keyword::databases_kw },
	{ "delete",    Keyword::delete_kw },
	{ "desc",      Keyword::desc_kw },
	{ "describe",  Keyword::describe_kw },
	{ "double",    Keyword::double_kw },
	{ "drop",      Keyword::drop_kw },
	{ "float",     Keyword::float_kw },
	{ "from",      Keyword::from_kw },
	{ "group",     Keyword::group_kw },
	{ "help",      Keyword::help_kw },
	{ "in",        Keyword::in_kw },
	{ "insert",    Keyword::insert_kw },
	{ "int",       Keyword::integer_kw },
	{ "integer",   Keyword::integer_kw },
	{ "into",      Keyword::into_kw },
	{ "key",       Keyword::key_kw },
	{ "not",       Keyword::not_kw },
	{ "null",      Keyword::null_kw },
	{ "or",        Keyword::or_kw },
	{ "order",     Keyword::order_kw },
	{ "primary",   Keyword::primary_kw },
	{ "quit",      Keyword::quit_kw },
	{ "select",    Keyword::select_kw },
	{ "set",       Keyword::set_kw },
	{ "show",      Keyword::show_kw },
	{ "table",     Keyword::table_kw },
	{ "tables",    Keyword::tables_kw },
	{ "unique",    Keyword::unique_kw },
	{ "update",    Keyword::update_kw },
	{ "use",       Keyword::use_kw },
	{ "values",    Keyword::values_kw },
	{ "varchar",   Keyword::varchar_kw },
	{ "version",   Keyword::version_kw },
	{ "where",     Keyword::where_kw }
};

bool isKeyword(const std::string& aWord) {
	return str2keyword.count(aWord) == 1;
}

Keyword toKeyword(const std::string& aWord) {
	return isKeyword(aWord) ? str2keyword.at(aWord) : Keyword::unknown_kw;
}

}