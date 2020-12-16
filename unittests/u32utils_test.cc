#include <gtest/gtest.h>
#include <mime/mime.hh>
#include <mime/u32utils.hh>
#include <string>

using namespace std::literals;

TEST(u32utils, isspace) {
    EXPECT_EQ(mime::u32::isspace(L' '), true);
    EXPECT_EQ(mime::u32::isspace(L'w'), false);
}

TEST(u32utils, ltrim) {
    auto s = L"  hello"s;
    mime::u32::ltrim(s);
    EXPECT_EQ(s, L"hello");
    mime::u32::ltrim(s);
    EXPECT_EQ(s, L"hello");

    s = L"  hello"s;
    mime::text t{begin(s), end(s)};
    mime::u32::ltrim(t);
    EXPECT_EQ(mime::to_string(t), "hello");
    mime::u32::ltrim(s);
    EXPECT_EQ(mime::to_string(t), "hello");
}

TEST(u32utils, rtrim) {
    auto s = L"hello  "s;
    mime::u32::rtrim(s);
    EXPECT_EQ(s, L"hello");
    mime::u32::rtrim(s);
    EXPECT_EQ(s, L"hello");

    s = L"hello  "s;
    mime::text t{begin(s), end(s)};
    mime::u32::rtrim(t);
    EXPECT_EQ(mime::to_string(t), "hello");
    mime::u32::rtrim(s);
    EXPECT_EQ(mime::to_string(t), "hello");
}

TEST(u32utils, trim) {
    auto s = L"  hello  "s;
    mime::u32::trim(s);
    EXPECT_EQ(s, L"hello");
    mime::u32::trim(s);
    EXPECT_EQ(s, L"hello");

    s = L"  hello  "s;
    mime::text t{begin(s), end(s)};
    mime::u32::trim(t);
    EXPECT_EQ(mime::to_string(t), "hello");
    mime::u32::trim(s);
    EXPECT_EQ(mime::to_string(t), "hello");
}

TEST(u32utils, toupper) {
    EXPECT_EQ(mime::u32::toupper(L'2'), L'2');
    EXPECT_EQ(mime::u32::toupper(L'w'), L'W');
    EXPECT_EQ(mime::u32::toupper(L'W'), L'W');
}

TEST(u32utils, lower) {
    EXPECT_EQ(mime::u32::tolower(L'2'), L'2');
    EXPECT_EQ(mime::u32::tolower(L'w'), L'w');
    EXPECT_EQ(mime::u32::tolower(L'W'), L'w');
}
