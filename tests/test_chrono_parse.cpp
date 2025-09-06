#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "mgutility/chrono/parse.hpp"

template <typename T>
auto to_milliseconds(T time_point) -> std::chrono::milliseconds {
  return std::chrono::duration_cast<std::chrono::milliseconds>(time_point.time_since_epoch());
}

TEST_CASE("testing the iso8601 parsing") {
  using namespace std::chrono_literals;

  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T}", "2023-04-30T16:22:18")) == 1682871738000ms);
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T16:22:18Z")) == 1682871738000ms);
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T18:22:18+0200")) == 1682871738000ms);
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T%z}", "2023-04-30T16:22:18-0200")) == 1682878938000ms);
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f}", "2023-04-30T16:22:18.500")) == 1682871738500ms);
  CHECK(to_milliseconds(mgutility::chrono::parse("{:%FT%T.%f%z}", "2023-04-30T16:22:18.500+0100")) == 1682868138500ms);

  REQUIRE_THROWS(mgutility::chrono::parse("{:%FT%T", "2023-04-30T16:22:18"));
  REQUIRE_THROWS(mgutility::chrono::parse("%FT%T}", "2023-04-30T16:22:18"));
  REQUIRE_THROWS(mgutility::chrono::parse("{%F %T}", "2023-04-30T16:22:18"));
  REQUIRE_THROWS(mgutility::chrono::parse("{:%F %T}", "2023-04-30T16:22:18"));
}