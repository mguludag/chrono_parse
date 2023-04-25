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

## [Performance](https://quick-bench.com/q/P2FgpMZs9Cv9pq8vk8G-es_q9G4)
- Performance is ~20x faster than `std::get_time` and ~70x faster than `std::get_time` + `std::mktime`.

![P2FgpMZs9Cv9pq8vk8G-es_q9G4](https://user-images.githubusercontent.com/12413639/232605091-564e4e85-df24-489e-b582-8181bab2b14e.png)



