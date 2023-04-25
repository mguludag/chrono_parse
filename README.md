# get_time
parse date and times with {fmt} style into`std::chrono::time_point` {WIP} 

## Usage
```C++
#include "get_time.hpp"

// {fmt} for printing
#include <fmt/chrono.h>
#include <fmt/format.h>

int main()
{
  const auto ch_time = mgutility::get_time("{:%F %T.%f %z}", "2023-04-16 00:05:23.862 +0100");
  
  fmt::print("{:%F %T}\n", ch_time);
}
```

## Notes
- Currently not supported all format specifiers (at least ISO-8601 capable now)
- Requires C++17 and upper

## [Performance](https://quick-bench.com/q/P2FgpMZs9Cv9pq8vk8G-es_q9G4)
- Performance is ~20x faster than `std::get_time` and ~70x faster than `std::get_time` + `std::mktime`.

![P2FgpMZs9Cv9pq8vk8G-es_q9G4](https://user-images.githubusercontent.com/12413639/232605091-564e4e85-df24-489e-b582-8181bab2b14e.png)



