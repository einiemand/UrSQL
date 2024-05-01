#include "exception/Exception.hpp"

namespace ursql {

Exception::Exception(std::string what) : what_(std::move(what)) {}

const char* Exception::what() const noexcept {
    return what_.c_str();
}

}  // namespace ursql
