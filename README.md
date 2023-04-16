# get_time
parse date and times with {fmt} style into `std::tm` and `std::chrono::time_point` {WIP} 

## Usage
```C++
#include "get_time.hpp"

// {fmt} for printing
#include <fmt/chrono.h>
#include <fmt/format.h>

int main()
{
  const auto time = mgutility::get_time<std::tm>("{:%F %T}", "2023-04-16 00:05:23");
  
  fmt::print("{:%F %T}\n", time);
  
  const auto ch_time = mgutility::get_time<std::chrono::system_clock::time_point>("{:%F %T}", "2023-04-16 00:05:23");
  
  fmt::print("{:%F %T}\n", ch_time);
}
```

## Notes
- Currently not supported all format specifiers (at least ISO-8601 capable now)
- Requires C++17 and upper

## [Performance](https://quick-bench.com/q/iaSDXnfdNtT5zlDPrJB0w6xkZfs)
- Performance is ~15-20x faster than `std::get_time`
- ![IzoxBbHDmZX3u8dVXnK-4IJrK7I](https://user-images.githubusercontent.com/12413639/232256177-375e48c7-7456-4c92-b997-c4d4b7c1f39e.png)

