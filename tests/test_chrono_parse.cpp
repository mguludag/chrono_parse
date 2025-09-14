#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "mgutility/chrono/parse.hpp"
#include <chrono>

// trunk-ignore-all(clang-format)

template <typename T>
auto to_milliseconds(T time_point) -> std::chrono::milliseconds {
  return std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
}

TEST_CASE("Basic ISO 8601 Parsing") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-04-30T16:22:18")) == milliseconds(1682871738000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2022-12-31T23:59:59")) == milliseconds(1672531199000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-01-01T00:00:00")) == milliseconds(1672531200000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-05-01T00:00:00")) == milliseconds(1682899200000));
}

TEST_CASE("Timezone Offset Handling") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T16:22:18Z")) == milliseconds(1682871738000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T18:22:18+0200")) == milliseconds(1682871738000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T16:22:18-0200")) == milliseconds(1682878938000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2016-02-29T05:00:00-0000")) == milliseconds(1456722000000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2016-02-29T23:59:59+0000")) == milliseconds(1456790399000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2016-02-29T12:00:00-1200")) == milliseconds(1456790400000));
}

TEST_CASE("AM/PM Handling") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T12:00:00 AM")) == milliseconds(1682812800000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T12:00:00 PM")) == milliseconds(1682856000000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T11:59:59 PM")) == milliseconds(1682899199000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T01:00:00 AM")) == milliseconds(1682816400000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T01:00:00 PM")) == milliseconds(1682859600000));
}

TEST_CASE("Fractional Seconds") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.1")) == milliseconds(1682871738100)); // 100ms
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.123")) == milliseconds(1682871738123)); // 123ms
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.123456")) == milliseconds(1682871738123)); // 123ms (truncated)
}

TEST_CASE("Combined AM/PM, Fractional Seconds, and Timezone") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f %p}", "2023-04-30T11:59:59.500 PM")) == milliseconds(1682899199500));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f %p}", "2023-04-30T01:20:00.123 AM")) == milliseconds(1682817600123));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f %p %z}", "2023-04-30T11:59:59.500 PM +0100")) == milliseconds(1682895599500));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f %p %z}", "2023-04-30T01:20:00.123 AM -0200")) == milliseconds(1682824800123));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T %p %z}", "2023-04-30T01:20:00 PM +0200")) == milliseconds(1682853600000));
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T %p %z}", "2023-04-30T01:20:00 AM -0200")) == milliseconds(1682824800000));
}

TEST_CASE("Leap Year and Date Boundaries") {
  using std::chrono::milliseconds;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2020-02-29T12:00:00")) == milliseconds(1582977600000)); // Leap year
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2021-03-01T00:00:00")) == milliseconds(1614556800000)); // Feb 28 + 1 day
}

TEST_CASE("Error Handling") {
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T", "2023-04-30T16:22:18")); // Malformed format
  REQUIRE_THROWS(mgutility::chrono::parse("%FT%T}", "2023-04-30T16:22:18")); // Malformed format
  REQUIRE_THROWS(mgutility::chrono::parse("{%F %T}", "2023-04-30T16:22:18")); // Malformed format
  REQUIRE_THROWS(mgutility::chrono::parse("{:%F %T}", "2023-04-30T16:22:18")); // Format mismatch
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T}", "2021-02-29T12:00:00")); // Invalid date (non-leap year)
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T}", "2023-04-31T12:00:00")); // Invalid date (April 31)
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T}", "not-a-date")); // Invalid format
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T13:00:00 AM")); // Hour > 12
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T12:00:00 XM")); // Invalid AM/PM
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%H:%M:%S %p}", "2023-04-30T12:00:00")); // Missing AM/PM
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.")); // No digits after decimal
  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.A")); // Invalid fraction
}