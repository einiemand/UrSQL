#pragma once
#include <format>
#include <iostream>

namespace ursql {

extern std::ostream& out;
extern std::ostream& err;

}  // namespace ursql

namespace std {

template<typename EnumT>
struct formatter<EnumT, enable_if_t<is_enum_v<EnumT>, char>>
    : formatter<underlying_type_t<EnumT>> {
    template<typename FormatContext>
    decltype(auto) format(EnumT e, FormatContext& ctx) const {
        return formatter<underlying_type_t<EnumT>>::format(
          static_cast<underlying_type_t<EnumT>>(e), ctx);
    }
};

}  // namespace std
