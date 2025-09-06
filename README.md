# chrono_parse

parse date and times with {fmt} style into`std::chrono::time_point` {WIP}

## [Usage](https://godbolt.org/z/PE3s1Y35d)

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

## [Performance](https://quick-bench.com/q/ry5gUzPplSBQenbLVBqRt1ns2-k)

- Performance is ~50x faster than `std::get_time` + `std::mktime`.

![ry5gUzPplSBQenbLVBqRt1ns2-k](https://user-images.githubusercontent.com/12413639/234938992-93cd1cb0-3a17-4466-99ae-b08cd3d3c8ff.png)
