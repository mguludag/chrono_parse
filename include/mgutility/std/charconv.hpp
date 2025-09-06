/*
MIT License

Copyright (c) 2024 mguludag

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef MGUTILITY_STD_CHARCONV_HPP
#define MGUTILITY_STD_CHARCONV_HPP

// trunk-ignore-all(clang-format)

#include "mgutility/_common/definitions.hpp"
#include <system_error>

#if MGUTILITY_CPLUSPLUS >= 201703L && !defined(CLANG_NO_CHARCONV)
#include <charconv>
#endif

namespace mgutility {

#if MGUTILITY_CPLUSPLUS < 201703L || defined(CLANG_NO_CHARCONV)

/**
 * @brief Result structure for from_chars function.
 */
struct from_chars_result {
  const char
      *ptr;     ///< Pointer to the character after the last parsed character.
  std::errc ec; ///< Error code indicating success or failure.
};

/**
 * @brief Converts a character to its integer equivalent.
 *
 * @param c The character to convert.
 * @return int The integer value of the character, or -1 if the character is not
 * a digit.
 */
constexpr auto char_to_int(char c) noexcept -> int {
  return (c >= '0' && c <= '9') ? c - '0' : -1;
}

/**
 * @brief Parses an integer from a character range.
 *
 * @param first Pointer to the first character of the range.
 * @param last Pointer to one past the last character of the range.
 * @param value Reference to an integer where the parsed value will be stored.
 * @return from_chars_result The result of the parsing operation.
 */
template <typename T = int32_t>
MGUTILITY_CNSTXPR auto from_chars(const char *first, const char *last,
                                  T &value) noexcept -> from_chars_result {
  T result = 0;
  bool negative = false;
  const char *it = first;

  if (it == last) {
    return {first, std::errc::invalid_argument};
  }

  if (*it == '-') {
    negative = true;
    ++it;
    if (it == last) {
      return {first, std::errc::invalid_argument};
    }
  }

  for (; it != last; ++it) {
    int digit = char_to_int(*it);
    if (digit == -1) {
      break;
    }
    result = result * 10 + digit;
  }

  if (it == first || (negative && it == first + 1)) {
    return {first, std::errc::invalid_argument};
  }

  value = negative ? -result : result;
  return {it, std::errc{}};
}
#else

using from_chars_result = std::from_chars_result;
using std::from_chars;

#endif

} // namespace mgutility

#endif // MGUTILITY_STD_CHARCONV_HPP