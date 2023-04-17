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

## [Performance](https://quick-bench.com/q/7ZBif24sf4xzkCvcJW5eKc52k4I)
- Performance is ~20x faster than `std::get_time` and ~70x faster than `std::get_time` + `std::mktime`.

![YQGFBkGchZxGE5NmE6_6pCtwfRs](https://user-images.githubusercontent.com/12413639/232600915-000eedc2-8fb7-448f-9266-1fad0e751fa2.png)


