#include <gtest/gtest.h>
#include <internal/chai.hh>
#include <string>

using namespace std::string_literals;

TEST(Chai, Good) {
    char *a[] = {strdup("mime"), strdup("testdata/test.mime")};
    EXPECT_EQ(mime::run(2, a), 0);
}

TEST(Chai, Fail) {
    char *a[] = {strdup("mime"), strdup("testdata/test.mime"), strdup("--fail")};
    EXPECT_EQ(mime::run(3, a), -42);
}

TEST(Chai, Bad) {
    char *a[] = {strdup("mime")};
    EXPECT_EQ(mime::run(1, a), 1);
}
