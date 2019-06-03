#include "Error.hpp"
#include <unordered_map>

namespace UrSQL {

void show_error(Error err, const char* const what, std::ostream& output) {
	static std::unordered_map<Error, const char* const> err2msg{

	};
	output << err2msg.at(err);
	if (what) {
		output << ": " << what;
	}
	output << '\n';
}

}