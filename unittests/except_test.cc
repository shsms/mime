#include <gtest/gtest.h>
#include <mime/except.hh>

TEST(Except, Exit) {
    int code = 0;
    try {
        throw mime::exit_exception(-42);
    } catch (const mime::exit_exception &e) {
        code = e.get_code();
    }
    EXPECT_EQ(code, -42);
}
