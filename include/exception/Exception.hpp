#pragma once

#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
#include <string>

namespace ursql {

using Traced =
  boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

template<class E>
[[noreturn]] void throw_traced(const E& e) {
    throw boost::enable_error_info(e)
      << Traced(boost::stacktrace::stacktrace());
}

class Exception : public std::exception {
public:
    explicit Exception(std::string what);
    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override;

protected:
    const std::string what_;
};

}  // namespace ursql
