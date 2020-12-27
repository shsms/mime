#include <gtest/gtest.h>
#include <mime/chai.hh>
#include <string>

using namespace std::string_literals;

TEST(Chai, BufferGood) {
    char *a[] = {strdup("mime"), strdup("testdata/test.mime")};
    EXPECT_EQ(mime::run(2, a), 0);
}

TEST(Chai, BufferBad) {
    char *a[] = {strdup("mime")};
    EXPECT_EQ(mime::run(1, a), 1);
}
