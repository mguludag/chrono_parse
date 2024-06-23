#include "mgutility/chrono/parse.hpp"

// {fmt} for printing
#include <fmt/chrono.h>
#include <fmt/format.h>

int main() {
  const auto chrono_time =
      mgutility::chrono::parse("{:%FT%T.%f%z}", "2023-04-16T00:05:23.999+0100");

  fmt::print("{:%F %T}\n", chrono_time); // prints 2023-04-15 23:05:23.999000000
                                         // ({fmt} trunk version)
}