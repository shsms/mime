#include <gtest/gtest.h>
#include "chai.hh"

TEST(Chai, Buffer) {
    chaiscript::ChaiScript chai;
    add_methods(chai);
    chai.eval(chai_init());

    chai.eval(R"(def assert(x, msg)
{
  if (!x)
  {
    print("ERROR: assert failed: " + msg);
    assert_failure(); // indirect throw
  }
})");

    chai.eval(R"(
var b = open("testdata/open.csv");
assert(b.is_new() == false, "is_new");

b.find("c");
assert(b.get_pos() == 5, "get_pos");
)");
}
