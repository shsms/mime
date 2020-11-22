#include <gtest/gtest.h>
#include <chaiscript/chaiscript.hpp>
#include "../src/chai.hh"
#include <string>

using namespace std::string_literals;

TEST(Chai, Buffer) {
    auto assert = R"(def assert(x, msg)
{
  if (!x)
  {
    print("ERROR: assert failed: " + msg);
    assert_failure(); // indirect throw
  }
})"s;

    auto chai_tests = R"(
var b = open("testdata/open.csv");
assert(b.is_new() == false, "is_new");

b.find("c");
assert(b.get_pos() == 5, "get_pos");
)"s;

    run_str(assert + chai_tests);
}
