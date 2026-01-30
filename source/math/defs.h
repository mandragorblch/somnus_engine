#pragma once
#include <expected>
#include <system_error>
#include <utility>

namespace smns {
namespace defs {
// alias for real type
using real = long double;

enum class ParseError {
  None,
  NaN
};

consteval std::expected<real, ParseError> parse_at_compile_time(
    const char* buffer, size_t sz) {
  real _real{};
  char ch;
  size_t it = 0uz;
  for (; it < sz; ++it) {
    ch = buffer[it];
    if (ch == '.') break;
    if (ch < '0' || ch > '9')
      return std::unexpected(ParseError::NaN);
    _real *= 10;
    _real += ch - '0';
  }

  if (ch == '.') ++it;

  real dig_pos = 1;
  for (; it < sz; ++it) {
    ch = buffer[it];
    if (ch < '0' || ch > '9')
      return std::unexpected(ParseError::NaN);
    dig_pos *= 10;
    _real += (ch - '0') / dig_pos;
  }

  return _real;
}

template <char... real_chars>
consteval real operator"" _r() {
  constexpr char buffer[] = {real_chars..., '\0'};
  constexpr auto res = parse_at_compile_time(buffer, sizeof...(real_chars));
  static_assert(res.has_value(), "NaN");
  return res.value();
}
}  // namespace defs
}  // namespace smns