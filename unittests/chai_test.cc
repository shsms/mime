#include <mime/chai.hh>
#include <gtest/gtest.h>
#include <string>

using namespace std::string_literals;

TEST(Chai, Buffer) { mime::run("testdata/test.mime"); }
