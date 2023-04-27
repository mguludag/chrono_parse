# get_time
parse date and times with {fmt} style into`std::chrono::time_point` {WIP} 

## [Usage](https://godbolt.org/z/cvhPxMMGq)
```C++
#include "get_time.hpp"

// {fmt} for printing
#include <fmt/chrono.h>
#include <fmt/format.h>

int main() {
    const auto chrono_time = mgutility::get_time("{:%FT%T.%f%z}", "2023-04-16T00:05:23.999+0100");

    fmt::print("{:%F %T}\n", chrono_time); // prints 2023-04-15 23:05:23.999000000 ({fmt} trunk version)
}
```

## Notes
- Currently not supported all format specifiers (at least ISO-8601 capable now)
- Requires C++17 and upper

## [Performance](https://quick-bench.com/q/ry5gUzPplSBQenbLVBqRt1ns2-k)
- Performance is ~50x faster than `std::get_time` + `std::mktime`.

![ry5gUzPplSBQenbLVBqRt1ns2-k](https://user-images.githubusercontent.com/12413639/234938992-93cd1cb0-3a17-4466-99ae-b08cd3d3c8ff.png)




