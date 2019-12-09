/* Running this tests
~~~~~~~~~~~~~~~~~~~~~~

$ cd tests
$ g++ unit.cpp
$ ./a.out
*/

#define CATCH_CONFIG_MAIN

#include "catch.hpp"
// #include "../include/format.h"
#include "../src/format.cpp"

TEST_CASE("Test uptime formatter", "[classic]") {
  REQUIRE(Format::ElapsedTime(60) == "0:1:0\n");
  REQUIRE(Format::ElapsedTime(130) == "0:2:10\n");
  REQUIRE(Format::ElapsedTime(1432) == "0:23:52\n");
}
