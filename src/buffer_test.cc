#include <gtest/gtest.h>
#include "buffer.hh"
#include <filesystem>
#include <string>

using namespace std::literals;

TEST(BufferOpen, UnnamedFile) {
    auto r = mime::open("");
    EXPECT_EQ(r.get_bool(), true);  // signifying a new file.
    EXPECT_EQ(r.get_buffer().contents.size(), 0);
}

TEST(BufferOpen, ExistingFile) {
    auto r = mime::open("testdata/open.csv");
    EXPECT_EQ(r.get_bool(), false);
    EXPECT_EQ(r.get_buffer().contents.size(), 18);
}

TEST(BufferOpen, NewFile) {
    auto r = mime::open("testdata/newfile");
    EXPECT_EQ(r.get_bool(), true);
    EXPECT_EQ(r.get_buffer().contents.size(), 0);
}

TEST(BufferSave, NewFile) {
    auto r = mime::open("testdata/newfile");
    EXPECT_EQ(r.get_bool(), true);
    r.b = mime::paste(r.b, 0, "hello");
    save(r.b);
    r = mime::open("testdata/newfile");
    EXPECT_EQ(r.get_bool(), false);
    EXPECT_EQ(r.get_buffer().contents.size(), 5);
    std::filesystem::remove("testdata/newfile");
}

TEST(BufferFind, Present) {
    auto r = mime::open("testdata/open.csv");
    EXPECT_EQ(r.b.cursors[0].point, 0);

    r = mime::find(r.b, 0, "a"s, 0);
    EXPECT_EQ(r.success, true);
    EXPECT_EQ(r.b.cursors[0].point, 1);

    r = mime::find(r.b, 0, "b"s, 0);
    EXPECT_EQ(r.success, true);
    EXPECT_EQ(r.b.cursors[0].point, 3);

    r = mime::find(r.b, 0, "c\n1"s, 0);
    EXPECT_EQ(r.success, true);
    EXPECT_EQ(r.b.cursors[0].point, 7);
    
    r = mime::find(r.b, 0, "6\n"s, 0);
    EXPECT_EQ(r.success, true);
    EXPECT_EQ(r.b.cursors[0].point, 18);
}

TEST(BufferFind, Limited) {
    auto r = mime::open("testdata/open.csv");
    EXPECT_EQ(r.b.cursors[0].point, 0);
    
    r = mime::find(r.b, 0, "5,6"s, 10);
    EXPECT_EQ(r.success, false);
    EXPECT_EQ(r.b.cursors[0].point, 0);
    
    r = mime::find(r.b, 0, "5,6"s, 20);
    EXPECT_EQ(r.success, true);
    EXPECT_EQ(r.b.cursors[0].point, 17);
    
}

TEST(BufferFind, Missing) {
    auto r = mime::open("testdata/open.csv");
    EXPECT_EQ(r.b.cursors[0].point, 0);
    
    r = mime::find(r.b, 0, "zz"s, 0);
    EXPECT_EQ(r.success, false);
    EXPECT_EQ(r.b.cursors[0].point, 0);

    r = mime::find(r.b, 0, "b"s, 0);
    r = mime::find(r.b, 0, "zzz"s, 30);
    EXPECT_EQ(r.success, false);
    EXPECT_EQ(r.b.cursors[0].point, 3);

    r = mime::find(r.b, 0, ""s, 0);
    EXPECT_EQ(r.success, false);
    EXPECT_EQ(r.b.cursors[0].point, 3);
}

TEST(BufferFind, EmptyFile) {
    auto r = mime::open("");
    r = mime::find(r.b, 0, "zz"s, 0);
    EXPECT_EQ(r.success, false);
    EXPECT_EQ(r.b.cursors[0].point, 0);
}

// TEST(BufferRfind, Present) {
    
// }
