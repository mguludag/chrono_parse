/*
 * MIT License
 *
 * Copyright (c) 2023 Muhammed Galib Uludag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <cctype>
#include <charconv>
#include <chrono>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace mgutility {
namespace detail {

struct tm : std::tm {
    uint32_t tm_ms;
};

inline auto parse_integer(std::string_view str, uint32_t len, uint32_t& next,
                          uint32_t begin_offset = 0) -> int32_t {
    int32_t result{0};
    if (str.size() < len + next)
        throw std::invalid_argument("value is not convertible!");
    for (auto it{str.begin() + next + begin_offset};
         it != str.begin() + len + next - 2; ++it) {
        if (!std::isdigit(*it))
            throw std::invalid_argument("value is not convertible!");
    }
    auto error = std::from_chars(str.begin() + next + begin_offset,
                                 str.begin() + len + next, result);

    next = ++len + next;

    if (error.ec != std::errc())
        throw std::invalid_argument("value is not convertible!");
    return result;
}

inline constexpr auto check_range(int32_t value, int32_t min, int32_t max) {
    if (value < min || value > max) {
        throw std::out_of_range("value is out of range!");
    }
}

// inspired from https://sources.debian.org/src/tdb/1.2.1-2/libreplace/timegm.c/
inline constexpr auto mktime(std::tm& tm) -> std::time_t {
    auto is_leap = [](unsigned y) {
        y += 1900;
        return (y % 4) == 0 && ((y % 100) != 0 || (y % 400) == 0);
    };

    constexpr unsigned ndays[2][12] = {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

    std::time_t res{0};
    unsigned i{0};

    if (tm.tm_mon > 12 || tm.tm_mon < 0 || tm.tm_mday > 31 || tm.tm_min > 60 ||
        tm.tm_sec > 60 || tm.tm_hour > 24) {
        /* invalid tm structure */
        return 0;
    }

    for (i = 70; i < tm.tm_year; ++i) res += is_leap(i) ? 366 : 365;

    for (i = 0; i < tm.tm_mon; ++i) res += ndays[is_leap(tm.tm_year)][i];
    res += tm.tm_mday - 1;
    res *= 24;
    res += tm.tm_hour;
    res *= 60;
    res += tm.tm_min;
    res *= 60;
    res += tm.tm_sec;

    return res;
}

template <typename T>
std::enable_if_t<std::is_base_of_v<std::tm, T>, T> constexpr get_time(
    std::string_view format, std::string_view date_str) {
    int32_t count{0};
    uint32_t beg{0}, end{0};
    for (auto i{0}; i < format.size(); ++i) {
        switch (format[i]) {
            case '{':
                beg = i;
                ++count;
                break;
            case '}':
                end = i;
                --count;
                break;
        }
        if (beg < end)
            break;
        else if (count != 0 && end < beg)
            break;
    }

    if (format[beg + 1] != ':' && end - beg < 3 || count != 0)
        throw std::invalid_argument("invalid format string!");

    T tm{};

    auto handle_timezone = [&tm](int32_t offset) {
        const auto minute = offset % 100;
        const auto hour = offset / 100;
        if (offset < 0) {
            tm.tm_min + minute < 0 ? tm.tm_hour - 1 < 0 ? --tm.tm_mday,
                tm.tm_hour = 23 - tm.tm_hour : --tm.tm_hour : 0;
            tm.tm_min -= minute;
            tm.tm_min %= 60;

            tm.tm_hour + hour < 0 ? --tm.tm_mday, tm.tm_hour = 24 + hour 
                : tm.tm_hour += hour;
        } else {
            tm.tm_min + minute > 59 ? tm.tm_hour + 1 > 23 ? ++tm.tm_mday,
                ++tm.tm_hour %= 24 : ++tm.tm_hour : 0;
            tm.tm_min += minute;
            tm.tm_min %= 60;

            tm.tm_hour + hour > 23 ? ++tm.tm_mday,
                tm.tm_hour = (tm.tm_hour + hour) % 24,
                (tm.tm_mon > 10 ? ++tm.tm_mon : 0) : tm.tm_hour += hour;
        }
    };

    uint32_t next{0};

    for (auto i{beg}; i < end; ++i) {
        switch (format[i]) {
            case '%': {
                if (i + 1 >= format.size())
                    throw std::invalid_argument("invalid format string!");
                switch (format[i + 1]) {
                    case 'Y':
                        tm.tm_year = parse_integer(date_str, 4, next) % 1900;
                        break;
                    case 'm':
                        tm.tm_mon = parse_integer(date_str, 2, next) - 1;
                        check_range(tm.tm_mon, 0, 11);
                        break;
                    case 'd':
                        tm.tm_mday = parse_integer(date_str, 2, next);
                        check_range(tm.tm_mday, 1, 31);
                        break;
                    case 'F': {
                        tm.tm_year = parse_integer(date_str, 4, next) % 1900;
                        tm.tm_mon = parse_integer(date_str, 2, next) - 1;
                        tm.tm_mday = parse_integer(date_str, 2, next);
                        check_range(tm.tm_mon, 0, 11);
                        check_range(tm.tm_mday, 1, 31);
                    } break;
                    case 'H':
                        tm.tm_hour = parse_integer(date_str, 2, next);
                        check_range(tm.tm_hour, 0, 23);
                        break;
                    case 'M':
                        tm.tm_min = parse_integer(date_str, 2, next);
                        check_range(tm.tm_min, 0, 59);
                        break;
                    case 'S':
                        tm.tm_sec = parse_integer(date_str, 2, next);
                        check_range(tm.tm_sec, 0, 59);
                        break;
                    case 'T': {
                        tm.tm_hour = parse_integer(date_str, 2, next);
                        tm.tm_min = parse_integer(date_str, 2, next);

                        tm.tm_sec = parse_integer(date_str, 2, next);

                        check_range(tm.tm_hour, 0, 23);
                        check_range(tm.tm_min, 0, 59);

                        check_range(tm.tm_sec, 0, 59);
                    } break;
                    case 'f': {
                        tm.tm_ms = parse_integer(date_str, 3, next);
                        check_range(tm.tm_ms, 0, 999);
                        break;
                    }
                    case 'z': {
                        char sign{};
                        auto diff{0};
                        for (auto j{next - 1}; j < date_str.size(); ++j) {
                            if (date_str[j] == '-' || date_str[j] == '+') {
                                sign = date_str[j];
                                diff = j - next + 1;
                                break;
                            }
                        }
                        auto hour_offset_str =
                            std::string_view{date_str.begin() + next + diff,
                                             date_str.size() - 1};
                        auto pos = hour_offset_str.find(':');
                        auto offset{0};
                        if (pos < 3 && pos > 0) {
                            next = 0;
                            auto hour_offset =
                                parse_integer(hour_offset_str, 2, next);
                            auto min_offset =
                                parse_integer(hour_offset_str, 2, next);
                            offset = hour_offset * 100 + min_offset;
                        } else {
                            if (date_str.size() - next > 4)
                                throw std::invalid_argument(
                                    "value is not convertible!");
                            offset = parse_integer(
                                date_str, date_str.size() - next, next, diff);
                        }
                        check_range(offset, 0, 1200);
                        switch (sign) {
                            case '+':
                                handle_timezone(offset * -1);
                                break;
                            case '-':
                                handle_timezone(offset);
                                break;
                        }
                    } break;
                }
            } break;
            case ' ':
            case '-':
            case '/':
            case '.':
            case ':':
                if (i > 1 && format[i] != date_str[next - 1])
                    throw std::invalid_argument("value is not convertible!");
                break;
        }
    }

    return tm;
}

}  // namespace detail

auto constexpr get_time(std::string_view format, std::string_view date_str)
    -> std::chrono::system_clock::time_point {
    auto tm = detail::get_time<detail::tm>(format, date_str);
    auto time_t = mgutility::detail::mktime(tm);
    std::chrono::system_clock::time_point clock =
        std::chrono::system_clock::from_time_t(time_t);
    clock += std::chrono::milliseconds(tm.tm_ms);
    return clock;
}
}  // namespace mgutility
