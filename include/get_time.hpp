#include <charconv>
#include <chrono>
#include <type_traits>

namespace mgutility {
inline auto parse_integer(std::string_view str, uint32_t len, uint32_t& next,
                          uint32_t begin_offset = 0) -> int32_t {
    int32_t result{0};
    if (str.size() < len + next)
        throw std::invalid_argument("value is not convertible!1");
    for (auto it{str.begin() + next + begin_offset};
         it != str.begin() + len + next; ++it) {
        if (!std::isdigit(*it))
            throw std::invalid_argument("value is not convertible!2");
    }
    auto error = std::from_chars(str.begin() + next + begin_offset,
                                 str.begin() + len + next, result);

    next = ++len + next;
    if (error.ec != std::errc())
        throw std::invalid_argument("value is not convertible!3");
    return result;
}

inline constexpr auto check_range(int32_t value, int32_t min, int32_t max) {
    if (value < min || value > max)
        throw std::out_of_range("value is out of range!");
}

template <typename T>
std::enable_if_t<std::is_same_v<std::tm, T>, T> constexpr get_time(
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

    auto change_date = [&tm](int32_t offset) {
        const auto minute = offset % 100;
        const auto hour = offset / 100;
        if (offset < 0) {
            tm.tm_min + minute < 0 ? tm.tm_hour - 1 < 0 ? --tm.tm_mday,
                tm.tm_hour = 23 - tm.tm_hour : --tm.tm_hour : 0;
            tm.tm_min -= minute;
            tm.tm_min %= 60;

            tm.tm_hour + hour < 0      ? --tm.tm_mday,
                tm.tm_hour = 23 + hour : tm.tm_hour += hour;
        } else {
            tm.tm_min + minute > 59 ? tm.tm_hour + 1 > 23 ? ++tm.tm_mday,
                ++tm.tm_hour %= 24 : ++tm.tm_hour : 0;
            tm.tm_min += minute;
            tm.tm_min %= 60;

            tm.tm_hour + hour > 23                    ? ++tm.tm_mday,
                tm.tm_hour = (tm.tm_hour + hour) % 24 : tm.tm_hour += hour;
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
                        auto hour_offset_str = std::string_view{date_str.begin() + next + diff,
                                 date_str.end()};
                        auto pos = hour_offset_str.find(':');
                        auto offset{0};
                        if(pos != std::string::npos){
                            next = 0;
                            auto hour_offset = parse_integer(hour_offset_str, 2, next);
                            auto min_offset = parse_integer(hour_offset_str, 2, next);
                            offset = hour_offset * 100 + min_offset;
                        }
                        else{
                            if(date_str.size() - next > 4)
                                throw std::invalid_argument("value is not convertible!");
                            offset = parse_integer(
                            date_str, date_str.size() - next, next, diff);
                        }
                        switch (sign) {
                            case '+':
                                change_date(offset * -1);
                                break;
                            case '-':
                                change_date(offset);
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

template <typename T>
std::enable_if_t<std::is_same_v<std::chrono::system_clock::time_point, T>,
                 T> constexpr get_time(std::string_view format,
                                             std::string_view date_str) {
    auto tm = get_time<std::tm>(format, date_str);
    auto time_t = std::mktime(&tm);
    T clock = std::chrono::system_clock::from_time_t(time_t);
    return clock;
}
}
