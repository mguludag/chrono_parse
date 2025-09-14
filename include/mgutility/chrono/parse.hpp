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

#include <cctype>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <ratio>
#include <stdexcept>
#include <type_traits>

// NOLINTBEGIN(modernize-concat-nested-namespaces)
namespace mgutility {
namespace chrono {
namespace detail {
// NOLINTEND(modernize-concat-nested-namespaces)

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
template <typename T>
MGUTILITY_CNSTXPR auto parse_integer(T &result, mgutility::string_view str,
                                     uint32_t len, uint32_t &next,
                                     uint32_t begin_offset = 0) -> std::errc {
  auto error = mgutility::from_chars(str.data() + next + begin_offset,
                                     str.data() + len + next, result);

  next += ++len;

  return error.ec;
}

/**
 * @brief Returns the absolute value of an integer.
 *
 * @param value
 * @return constexpr int32_t
 */
constexpr int32_t abs(int32_t value) noexcept {
  return value >= 0 ? value : -value;
}

// NOLINTNEXTLINE
constexpr auto pow(int32_t base, int32_t exp) noexcept -> int32_t {
  // NOLINTNEXTLINE
  return exp < 0 ? 0 : exp == 0 ? 1 : base * pow(base, exp - 1);
}

/**
 * @brief Checks if a value is within a given range.
 *
 * @param value The value to check.
 * @param min The minimum acceptable value.
 * @param max The maximum acceptable value.
 * @throws std::out_of_range if the value is out of range.
 */
template <typename T>
MGUTILITY_CNSTXPR auto check_range(const T &value, const T &min, const T &max)
    -> std::errc {
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
auto MGUTILITY_CNSTXPR is_leap_year(int32_t year) -> bool {
  return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}

/**
 * @brief Returns the number of days in a given month of a given year.
 *
 * @param year
 * @param month
 * @return int32_t
 */
// NOLINTNEXTLINE
auto MGUTILITY_CNSTXPR days_in_month(int32_t year, int32_t month) -> int32_t {
  // NOLINTNEXTLINE
  constexpr int days_per_month[] = {31, 28, 31, 30, 31, 30,
                                    31, 31, 30, 31, 30, 31};
  if (month < 0 || month > 11) {
    return 0; // Invalid month
  }
  if (month == 1) { // February
    return is_leap_year(year) ? 29 : 28;
  }
  // NOLINTNEXTLINE
  return days_per_month[month];
}

/**
 * @brief Converts a tm structure to a time_t value.
 *
 * @param tm The tm structure to convert.
 * @return std::time_t The corresponding time_t value.
 * @throws std::out_of_range if any tm value is out of valid range.
 */
MGUTILITY_CNSTXPR auto mktime(std::time_t &result, std::tm &time_struct)
    -> std::errc {
  result = 0;

  // Check for out of range values in tm structure
  if (time_struct.tm_mon > 12 || time_struct.tm_mon < 0 ||
      time_struct.tm_mday > 31 || time_struct.tm_min > 60 ||
      time_struct.tm_sec > 60 || time_struct.tm_hour > 24) {
    return std::errc::result_out_of_range;
  }

  time_struct.tm_year += 1900;

  if (days_in_month(time_struct.tm_year, time_struct.tm_mon) <
      time_struct.tm_mday) {
    return std::errc::result_out_of_range;
  }

  // Calculate the number of days since 1970
  for (auto i{1970}; i < time_struct.tm_year; ++i) {
    result += is_leap_year(i) ? 366 : 365;
  }

  // Add the days for the current year
  for (auto i{0}; i < time_struct.tm_mon; ++i) {
    // NOLINTNEXTLINE
    result += days_in_month(time_struct.tm_year, i);
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
MGUTILITY_CNSTXPR auto handle_timezone(tm &time_struct, int32_t offset)
    -> std::errc {
  // Validate offset: HHMM format, minutes 0-59, hours 0-23
  const int32_t abs_offset = abs(offset);
  const int32_t minutes = abs_offset % 100;
  const int32_t hours = abs_offset / 100;
  if (minutes > 59 || hours > 23) {
    return std::errc::invalid_argument;
  }

  // Validate input tm structure (basic checks)
  if (time_struct.tm_mon < 0 || time_struct.tm_mon > 11 ||
      time_struct.tm_mday < 1 || time_struct.tm_year < 0) {
    return std::errc::invalid_argument;
  }

  // Apply offset (positive or negative)
  const int32_t total_minutes =
      time_struct.tm_min + (offset >= 0 ? minutes : -minutes);
  const int32_t total_hours =
      time_struct.tm_hour + (offset >= 0 ? hours : -hours);

  // Normalize minutes (-59 to 119 -> 0-59 with hour carry)
  time_struct.tm_min = total_minutes % 60;
  int minute_carry = total_minutes / 60;
  if (total_minutes < 0 && total_minutes % 60 != 0) {
    minute_carry -= 1;
    time_struct.tm_min += 60;
  }

  // Normalize hours (-23 to 47 -> 0-23 with day carry)
  time_struct.tm_hour = (total_hours + minute_carry) % 24;
  int day_carry = (total_hours + minute_carry) / 24;
  if (total_hours + minute_carry < 0 &&
      (total_hours + minute_carry) % 24 != 0) {
    day_carry -= 1;
    time_struct.tm_hour += 24;
  }

  // Normalize days, months, and years
  int days = time_struct.tm_mday + day_carry;
  int months = time_struct.tm_mon;
  int years = time_struct.tm_year;

  // Handle negative days
  while (days <= 0) {
    months -= 1;
    if (months < 0) {
      months += 12;
      years -= 1;
      if (years < 0) {
        return std::errc::result_out_of_range; // Year underflow
      }
    }
    days += days_in_month(years, months);
  }

  // Handle day overflow
  while (days > days_in_month(years, months)) {
    days -= days_in_month(years, months);
    months += 1;
    if (months > 11) {
      months -= 12;
      years += 1;
      if (years > 9999 - 1900) { // Avoid overflow (arbitrary limit)
        return std::errc::result_out_of_range;
      }
    }
  }

  // Update tm structure
  time_struct.tm_mday = days;
  time_struct.tm_mon = months;
  time_struct.tm_year = years;

  return std::errc{};
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
  error = check_range(result.tm_mday, 1,
                      days_in_month(result.tm_year, result.tm_mon));
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
  int32_t digits = 0;
  while (next + digits < date_str.size() &&
         mgutility::detail::is_digit(date_str[next + digits]) && digits < 9) {
    ++digits;
  }
  auto error = parse_integer(result.tm_ms, date_str, digits, next);
  if (error != std::errc{}) {
    return error;
  }
  result.tm_ms *= static_cast<uint32_t>(pow(10, 9 - digits));
  error = check_range(result.tm_ms, 0U, 999999999U);
  return error;
}

MGUTILITY_CNSTXPR auto parse_timezone_offset(detail::tm &result,
                                             string_view date_str,
                                             uint32_t &next) -> std::errc {
  std::errc error{};
  // NOLINTNEXTLINE [bugprone-inc-dec-in-conditions]
  if (next < date_str.size() && date_str[next] == 'Z') {
    error = handle_timezone(result, 0);
    return error;
  }

  if (next >= date_str.size() ||
      (date_str[next] != '+' && date_str[next] != '-')) {
    return std::errc::invalid_argument;
  }

  const char sign = date_str[next++];
  int32_t hour = 0;
  int32_t minute = 0;

  error = parse_integer(hour, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }

  --next;

  if (next < date_str.size() && date_str[next] == ':') {
    ++next;
  }

  error = parse_integer(minute, date_str, 2, next);
  if (error != std::errc{}) {
    return error;
  }

  const int32_t offset = (hour * 100) + minute;
  error = check_range(offset, 0, 1200);
  if (error != std::errc{}) {
    return error;
  }

  error = handle_timezone(result, sign == '+' ? -offset : offset);
  return error;
}

/**
 * @brief Parses AM/PM and adjusts the hour in the tm structure accordingly.
 *
 * @param result The tm structure to adjust.
 * @param date_str The date string containing AM/PM.
 * @param next The position of the next character to parse.
 * @return std::errc An error code indicating success or failure.
 */
MGUTILITY_CNSTXPR auto parse_am_pm(detail::tm &result, string_view date_str,
                                   uint32_t &next) -> std::errc {
  if (next + 2 > date_str.size() || result.tm_hour < 1 || result.tm_hour > 12) {
    return std::errc::invalid_argument;
  }
  if (date_str.substr(next, 2) == "AM") {
    if (result.tm_hour == 12) {
      result.tm_hour = 0; // 12 PM = 12, 12 AM = 0
    }
    next += 2;
  } else if (date_str.substr(next, 2) == "PM") {
    if (result.tm_hour != 12) {
      result.tm_hour += 12; // 1-11 PM = 13-23
    }
    next += 2;
  } else {
    return std::errc::invalid_argument;
  }
  ++next;
  return std::errc{};
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
  bool is_specifier = false;
  std::errc error{};

  for (std::size_t i = begin; i < end; ++i) {
    switch (format[i]) {
    case '%': {
      if (i + 1 >= format.size()) {
        return std::errc::invalid_argument;
      }

      if (is_specifier) {
        --next;
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
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_year(result, date_str, next);
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_month(result, date_str, next);
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
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
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_hour(result, date_str, next);
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_minute(result, date_str, next);
        // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
        error = parse_second(result, date_str, next);
      } break;
      case 'f':
        error = parse_fraction(result, date_str, next);
        break;
      case 'z':
        error = parse_timezone_offset(result, date_str, next);
        break;
      case 'p':
        error = parse_am_pm(result, date_str, next);
        break;
      default:
        return std::errc::invalid_argument;
      }
      if (error != std::errc{}) {
        return error;
      }
      ++i;
      is_specifier = true;
    }
      continue;
    case ' ': // Space separator
    case '-': // Dash separator
    case '/': // Slash separator
    case '.': // Dot separator
    case ':': // Colon separator
    case 'T': // 'T' separator
      if (i > 1 && format[i] != date_str[next - 1]) {
        return std::errc::invalid_argument;
      }
      break;
    }
    is_specifier = false;
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
  time_point = std::chrono::time_point_cast<typename Clock::duration>(
      Clock::from_time_t(time_t) + std::chrono::nanoseconds{time_struct.tm_ms});
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
