#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "mgutility/chrono/parse.hpp"
#include <chrono>

// trunk-ignore-all(clang-format)

template <typename T>
auto to_milliseconds(T time_point) -> std::chrono::milliseconds {
  return std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
}

TEST_CASE("testing the iso8601 parsing") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-04-30T16:22:18")) == milliseconds(1682871738000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T16:22:18Z")) == milliseconds(1682871738000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T18:22:18+0200")) == milliseconds(1682871738000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T16:22:18-0200")) == milliseconds(1682878938000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.500")) == milliseconds(1682871738500));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f%z}", "2023-04-30T16:22:18.500+0100")) == milliseconds(1682868138500));

  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T", "2023-04-30T16:22:18"));
  REQUIRE_THROWS(mgutility::chrono::parse("%FT%T}", "2023-04-30T16:22:18"));
  REQUIRE_THROWS(mgutility::chrono::parse("{%F %T}", "2023-04-30T16:22:18"));
  REQUIRE_THROWS(mgutility::chrono::parse("{:%F %T}", "2023-04-30T16:22:18"));
}