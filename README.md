# chrono_parse

parse date and times with {fmt} style into`std::chrono::time_point` {WIP}

## [Usage](https://godbolt.org/z/cTK4977vP)

```C++
#include "mgutility/chrono/parse.hpp"

// {fmt} for printing
#include <fmt/chrono.h>
#include <fmt/format.h>

int main() {
    const auto chrono_time = mgutility::chrono::parse("{:%FT%T.%f%z}", "2023-04-16T00:05:23.999+0100");

    fmt::print("{:%F %T}\n", chrono_time); // prints 2023-04-15 23:05:23.999000000 ({fmt} trunk version)
}
```

## Format specifiers

| Format Specifier | Explanation                                                        |
| ---------------- | ------------------------------------------------------------------ |
| `%Y`             | parses **year** as a decimal number, e.g. 2023                     |
| `%m`             | parses **month** as a decimal number, e.g. 05                      |
| `%d`             | parses **day** as a decimal number, e.g. 14                        |
| `%H`             | parses **hour** as a decimal number, e.g. 16                       |
| `%M`             | parses **minute** as a decimal number, e.g. 31                     |
| `%S`             | parses **second** as a decimal number, e.g. 59                     |
| `%F`             | parses **year-month-day** as an iso8601 date, e.g. 2023-05-04      |
| `%T`             | parses **hour:minute:second** as an iso8601 time, e.g. 16:31:59    |
| `%f`             | parses **milliseconds** as a decimal number, e.g. 869              |
| `%z`             | parses **timezone** as a decimal number, e.g. +0100 or -01:00 or Z |

## Notes

- Currently not supported all format specifiers (at least ISO-8601 capable now)

## [Performance](https://quick-bench.com/q/6O2Ctb9wRnkvx_kHeq40xbYJu6A)

- Performance is ~20x faster than `std::get_time` + `std::mktime`.

> <img width="1658" height="829" alt="FuYJ4VJsqNsVqQ2Z92zbyToPDAA" src="https://github.com/user-attachments/assets/947daa7e-0e53-473a-82d0-fb2867ef76db" />
> clang 17 libc++ c++23 -OFast

> <img width="1658" height="829" alt="37g7u6VRsTnA-NuvD_QuKdbo6TA" src="https://github.com/user-attachments/assets/c21cbaf8-134f-4d29-8b05-fa50d9e78098" />
> clang 17 libstdc++ c++23 -OFast

> <img width="1658" height="829" alt="6O2Ctb9wRnkvx_kHeq40xbYJu6A" src="https://github.com/user-attachments/assets/874f25d5-8368-434d-a722-b73f6e9dea0d" />
> gcc 13.2 libstdc++ c++23 -OFast



