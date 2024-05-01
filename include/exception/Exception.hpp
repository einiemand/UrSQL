#pragma once

#include <exception>
#include <string>

namespace ursql {

class Exception : public std::exception {
public:
    explicit Exception(std::string what);
    ~Exception() override = default;

    [[nodiscard]] const char* what() const noexcept override;

protected:
    const std::string what_;
};

}  // namespace ursql
