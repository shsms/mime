#include <gtest/gtest.h>
#include "u32utils.hh"
#include "buffer.hh"
#include <string>

using namespace std::literals;

TEST(u32utils, isspace) {
    EXPECT_EQ(mime::u32::isspace(U' '), true);
    EXPECT_EQ(mime::u32::isspace(U'w'), false);
}

TEST(u32utils, ltrim) {
    auto s = U"  hello"s;
    mime::u32::ltrim(s);
    EXPECT_EQ(s, U"hello");
    mime::u32::ltrim(s);
    EXPECT_EQ(s, U"hello");

    s = U"  hello"s;
    mime::text t{begin(s), end(s)};
    mime::u32::ltrim(t);
    EXPECT_EQ(mime::to_string(t), "hello");
    mime::u32::ltrim(s);
    EXPECT_EQ(mime::to_string(t), "hello");
}

TEST(u32utils, rtrim) {
    auto s = U"hello  "s;
    mime::u32::rtrim(s);
    EXPECT_EQ(s, U"hello");
    mime::u32::rtrim(s);
    EXPECT_EQ(s, U"hello");

    s = U"hello  "s;
    mime::text t{begin(s), end(s)};
    mime::u32::rtrim(t);
    EXPECT_EQ(mime::to_string(t), "hello");
    mime::u32::rtrim(s);
    EXPECT_EQ(mime::to_string(t), "hello");
}

TEST(u32utils, trim) {
    auto s = U"  hello  "s;
    mime::u32::trim(s);
    EXPECT_EQ(s, U"hello");
    mime::u32::trim(s);
    EXPECT_EQ(s, U"hello");

    s = U"  hello  "s;
    mime::text t{begin(s), end(s)};
    mime::u32::trim(t);
    EXPECT_EQ(mime::to_string(t), "hello");
    mime::u32::trim(s);
    EXPECT_EQ(mime::to_string(t), "hello");
}

TEST(u32utils, toupper) {
    EXPECT_EQ(mime::u32::toupper(U'2'), U'2');
    EXPECT_EQ(mime::u32::toupper(U'w'), U'W');
    EXPECT_EQ(mime::u32::toupper(U'W'), U'W');
}

TEST(u32utils, lower) {
    EXPECT_EQ(mime::u32::tolower(U'2'), U'2');
    EXPECT_EQ(mime::u32::tolower(U'w'), U'w');
    EXPECT_EQ(mime::u32::tolower(U'W'), U'w');
}

