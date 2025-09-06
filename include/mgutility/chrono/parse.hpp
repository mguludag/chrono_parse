/*
 * MIT License
 *
 * (c) 2023 Muhammed Galib Uludag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef MGUTILITY_CHRONO_PARSE_HPP
#define MGUTILITY_CHRONO_PARSE_HPP

// trunk-ignore-all(clang-format)

#include "mgutility/std/charconv.hpp"
#include "mgutility/std/string_view.hpp"

#include <array>
#include <cctype>
#include <chrono>
#include <stdexcept>
#include <type_traits>

//NOLINTBEGIN(modernize-concat-nested-namespaces)
namespace mgutility {
namespace chrono {
namespace detail {
//NOLINTEND(modernize-concat-nested-namespaces)

/**
 * @brief Extended tm structure with milliseconds.
 */
struct tm : std::tm {
  uint32_t tm_ms; ///< Milliseconds.
};

/**
 * @brief Parses an integer from a string view.
 *
 * @param str The string view to parse.
 * @param len The length of the string view.
 * @param next The position of the next character to parse.
 * @param begin_offset The offset to begin parsing from.
 * @return int32_t The parsed integer.
 * @throws std::invalid_argument if the value is not convertible.
 */
template <typename T = int32_t>
MGUTILITY_CNSTXPR auto parse_integer(T &result, mgutility::string_view str,
                                     uint32_t len, uint32_t &next,
                                     uint32_t begin_offset = 0) -> std::errc {
  auto error = mgutility::from_chars(str.data() + next + begin_offset,
                                     str.data() + len + next, result);

  next = ++len + next;

  return error.ec;
}

/**
 * @brief Checks if a value is within a given range.
 *
 * @param value The value to check.
 * @param min The minimum acceptable value.
 * @param max The maximum acceptable value.
 * @throws std::out_of_range if the value is out of range.
 */
 template <typename T = int32_t>
MGUTILITY_CNSTXPR auto check_range(const T& value, const int32_t& min, const int32_t& max) -> std::errc {
  if (value < min || value > max) {
    return std::errc::result_out_of_range;
  }
  return std::errc{};
}

/**
 * @brief Determines if a year is a leap year.
 *
 * @param year The year to check.
 * @return bool True if the year is a leap year, false otherwise.
 */
auto MGUTILITY_CNSTXPR is_leap_year(uint32_t year) -> bool {
  return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}

/**
 * @brief Converts a tm structure to a time_t value.
 *
 * @param tm The tm structure to convert.
 * @return std::time_t The corresponding time_t value.
 * @throws std::out_of_range if any tm value is out of valid range.
 */
MGUTILITY_CNSTXPR auto mktime(std::time_t &result, std::tm &time_struct) -> std::errc {
  MGUTILITY_CNSTXPR std::array<std::array<uint32_t, 12>, 2> num_of_days{
      {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,
        31}, // 365 days in a common year
       {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30,
        31}}}; // 366 days in a leap year

  result = 0;

  // Check for out of range values in tm structure
  if (time_struct.tm_mon > 12 || time_struct.tm_mon < 0 || time_struct.tm_mday > 31 || time_struct.tm_min > 60 ||
      time_struct.tm_sec > 60 || time_struct.tm_hour > 24) {
    return std::errc::result_out_of_range;
  }

  time_struct.tm_year += 1900;

  // Calculate the number of days since 1970
  for (auto i{1970}; i < time_struct.tm_year; ++i) {
    result += is_leap_year(i) ? 366 : 365;
  }

  // Add the days for the current year
  for (auto i{0}; i < time_struct.tm_mon; ++i) {
    //NOLINTNEXTLINE(readability-implicit-bool-conversion cppcoreguidelines-pro-bounds-constant-array-index)
    result += num_of_days[is_leap_year(time_struct.tm_year)][i];
  }

  result += time_struct.tm_mday - 1; // nth day since 1970
  result *= 24;
  result += time_struct.tm_hour;
  result *= 60;
  result += time_struct.tm_min;
  result *= 60;
  result += time_struct.tm_sec;

  return std::errc{};
}

/**
 * @brief Adjusts the tm structure for a given timezone offset.
 *
 * @param tm The tm structure to adjust.
 * @param offset The timezone offset in hours and minutes.
 */
MGUTILITY_CNSTXPR auto handle_timezone(tm &time_struct, int32_t offset) -> void {
  const auto minute = offset % 100;
  const auto hour = offset / 100;

  if (offset < 0) {
    if (time_struct.tm_min + minute < 0) {
      time_struct.tm_min += 60 - minute;
      time_struct.tm_hour -= 1;
      if (time_struct.tm_hour < 0) {
        time_struct.tm_hour += 24;
        time_struct.tm_mday -= 1;
      }
    } else {
      time_struct.tm_min += minute;
    }

    if (time_struct.tm_hour + hour < 0) {
      time_struct.tm_hour += 24 + hour;
      time_struct.tm_mday -= 1;
    } else {
      time_struct.tm_hour += hour;
    }
  } else {
    if (time_struct.tm_min + minute >= 60) {
      time_struct.tm_min -= 60 - minute;
      time_struct.tm_hour += 1;
      if (time_struct.tm_hour >= 24) {
        time_struct.tm_hour -= 24;
        time_struct.tm_mday += 1;
      }
    } else {
      time_struct.tm_min += minute;
    }

    if (time_struct.tm_hour + hour >= 24) {
      time_struct.tm_hour += hour - 24;
      time_struct.tm_mday += 1;
      if (time_struct.tm_mon == 11 && time_struct.tm_mday > 31) {
        time_struct.tm_mday = 1;
        time_struct.tm_mon = 0;
      } else if (time_struct.tm_mday > 30) {
        time_struct.tm_mday = 1;
        time_struct.tm_mon += 1;
      }
    } else {
      time_struct.tm_hour += hour;
    }
  }
}

// Free parsing functions
MGUTILITY_CNSTXPR auto parse_year(detail::tm &result, string_view date_str,
                                  uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_year, date_str, 4, next);
  result.tm_year %= 1900;
  return error;
}

MGUTILITY_CNSTXPR auto parse_month(detail::tm &result, string_view date_str,
                                   uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_mon, date_str, 2, next);
  result.tm_mon -= 1;
  return error;
}

MGUTILITY_CNSTXPR auto parse_day(detail::tm &result, string_view date_str,
                                 uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_mday, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }
  error = check_range(result.tm_mday, 1, 31);
  return error;
}

MGUTILITY_CNSTXPR auto parse_hour(detail::tm &result, string_view date_str,
                                  uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_hour, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }
  error = check_range(result.tm_hour, 0, 23);
  return error;
}

MGUTILITY_CNSTXPR auto parse_minute(detail::tm &result, string_view date_str,
                                    uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_min, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }
  error = check_range(result.tm_min, 0, 59);
  return error;
}

MGUTILITY_CNSTXPR auto parse_second(detail::tm &result, string_view date_str,
                                    uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_sec, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }
  error = check_range(result.tm_sec, 0, 59);
  return error;
}

MGUTILITY_CNSTXPR auto parse_fraction(detail::tm &result, string_view date_str,
                                      uint32_t &next) -> std::errc {
  auto error = parse_integer(result.tm_ms, date_str, 3, next);
  if (error != std::errc{}) {
    return error;
  }
  error = check_range(result.tm_ms, 0, 999);
  return error;
}

MGUTILITY_CNSTXPR auto parse_timezone_offset(detail::tm &result, string_view date_str,
                       uint32_t &next) -> std::errc {
  std::errc error{};
  if (--next < date_str.size() && date_str[next] == 'Z') {
    handle_timezone(result, 0);
    return error;
  }

  if (next >= date_str.size() || (date_str[next] != '+' && date_str[next] != '-')) {
    return std::errc::invalid_argument;
  }

  const char sign = date_str[next++];
  int32_t hour = 0;
  int32_t minute = 0;

  error = parse_integer(hour, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }

  if (next < date_str.size() && date_str[next] == ':') {
    ++next;
  }

  error = parse_integer(minute, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }

  const int32_t offset = hour * 100 + minute;
  error = check_range(offset, 0, 1200);
  if (error != std::errc{}) {
    return error;
  }

  handle_timezone(result, sign == '+' ? -offset : offset);
  return error;
}

/**
 * @brief Parses a date and time string according to a specified format.
 *
 * @param format The format string.
 * @param date_str The date and time string to parse.
 * @return detail::tm The parsed time structure.
 * @throws std::invalid_argument if the format string is invalid or parsing
 * fails.
 */
MGUTILITY_CNSTXPR auto get_time(detail::tm &result, string_view format,
                                string_view date_str) -> std::errc {
  const std::size_t begin = format.find('{');
  const std::size_t end = format.find('}');
  if (begin == string_view::npos || end == string_view::npos || begin >= end) {
    return std::errc::invalid_argument;
  }

  if (format[begin + 1] != ':' || (end - begin < 3)) {
    return std::errc::invalid_argument;
  }

  uint32_t next = 0;
  std::errc error{};

  for (std::size_t i = begin; i < end; ++i) {
    switch (format[i]) {
    case '%': {
      if (i + 1 >= format.size()) {
        return std::errc::invalid_argument;
      }
      switch (format[i + 1]) {
      case 'Y':
        error = parse_year(result, date_str, next);
        break;
      case 'm':
        error = parse_month(result, date_str, next);
        break;
      case 'd':
        error = parse_day(result, date_str, next);
        break;
      case 'F': {
        //NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_year(result, date_str, next);
        //NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_month(result, date_str, next);
        //NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_day(result, date_str, next);
      } break;
      case 'H':
        error = parse_hour(result, date_str, next);
        break;
      case 'M':
        error = parse_minute(result, date_str, next);
        break;
      case 'S':
        error = parse_second(result, date_str, next);
        break;
      case 'T': {
        //NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_hour(result, date_str, next);
        //NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_minute(result, date_str, next);
        //NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_second(result, date_str, next);
      } break;
      case 'f':
        error = parse_fraction(result, date_str, next);
        break;
      case 'z': {
        error = parse_timezone_offset(result, date_str, next);
      } break;
      default:
        return std::errc::invalid_argument;
      }
      if (error != std::errc{}) {
        return error;
      }
    } break;
    case ' ': // Space separator
    case '-': // Dash separator
    case '/': // Slash separator
    case '.': // Dot separator
    case ':': // Colon separator
      if (i > 1 && format[i] != date_str[next - 1]) {
        return std::errc::invalid_argument;
      }
      break;
    }
  }

  return std::errc{};
}

} // namespace detail

/**
 * @brief Parses a date and time string into a std::chrono::time_point of the
 * specified clock type.
 *
 * @tparam Clock The clock type (defaults to std::chrono::system_clock).
 * @param time_point The time point to populate.
 * @param format The format string.
 * @param date_str The date and time string to parse.
 * @return std::error_code An error code indicating success or failure.
 */
template <typename Clock = std::chrono::system_clock>
auto parse(typename Clock::time_point &time_point, string_view format,
           string_view date_str) -> std::error_code {
  detail::tm time_struct{};
  auto error = detail::get_time(time_struct, format, date_str);
  if (error != std::errc{}) {
    return std::make_error_code(error);
  }
  std::time_t time_t{};
  error = detail::mktime(time_t, time_struct);
  if (error != std::errc{}) {
    return std::make_error_code(error);
  }
  time_point = Clock::from_time_t(time_t);
  time_point += std::chrono::milliseconds(time_struct.tm_ms);
  return std::error_code{};
}

/**
 * @brief Parses a date and time string into a std::chrono::time_point of the
 * specified clock type.
 *
 * @tparam Clock The clock type (defaults to std::chrono::system_clock).
 * @param format The format string.
 * @param date_str The date and time string to parse.
 * @throw std::system_error if parsing fails
 * @return Clock::time_point The parsed time point.
 */
template <typename Clock = std::chrono::system_clock>
auto parse(string_view format, string_view date_str) ->
    typename Clock::time_point {
  typename Clock::time_point time_point{};
  auto error = parse<Clock>(time_point, format, date_str);
  if (error) {
    throw std::system_error(error);
  }
  return time_point;
}

} // namespace chrono
} // namespace mgutility

#endif // MGUTILITY_CHRONO_PARSE_HPP
