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


  // Leap year: Feb 29 should work in a leap year
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2020-02-29T12:00:00")) == milliseconds(1582977600000));

// Non-leap year: Feb 29 should not exist
REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T}", "2021-02-29T12:00:00"));

// Leap year, with timezone offset: UTC should equal local time with -0000
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2016-02-29T05:00:00-0000")) == milliseconds(1456722000000));

// February 28 + 1 day (should rollover to Mar 1 on non-leap year)
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2021-03-01T00:00:00")) == milliseconds(1614556800000));

// Year rollover: Dec 31 to Jan 1
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2022-12-31T23:59:59")) == milliseconds(1672531199000));
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-01-01T00:00:00")) == milliseconds(1672531200000));

// Month boundary rollover: Apr 30 + 1 day == May 1
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-05-01T00:00:00")) == milliseconds(1682899200000));

// Invalid date: April 31
REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T}", "2023-04-31T12:00:00"));

// Edge timezone: Feb 29 with offset
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2016-02-29T23:59:59+0000")) == milliseconds(1456790399000));
CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2016-02-29T12:00:00-1200")) == milliseconds(1456790400000));

// Invalid date/time format
REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T}", "not-a-date"));
}