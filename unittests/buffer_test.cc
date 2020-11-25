#include <filesystem>
#include <gtest/gtest.h>
#include <mime/mime.hh>
#include <string>

using namespace std::literals;

class BufferTest : public ::testing::Test {
  protected:
    void SetUp() override {
        ascii = mime::buffer("testdata/open.csv");
        unicode = mime::buffer("testdata/unicode.csv");
    }

    mime::buffer ascii;
    mime::buffer unicode;
};

// TEST_F(BufferTest, ReplaceNotFound) {
//     auto r = mime::replace(ascii, 0, "", "c,b", 0);
//     EXPECT_EQ(r.n, 0);

//     r = mime::replace(ascii, 0, "zzwwd", "c,b", 0);
//     EXPECT_EQ(r.n, 0);
// }

// TEST_F(BufferTest, ReplaceFound) {
//     auto r = mime::replace(ascii, 0, "b,c", "c,b", 0);
//     EXPECT_EQ(r.n, 1);
//     EXPECT_STREQ(mime::to_string(r.b.contents).c_str(), "a,c,b\n1,2,3\n4,5,6\n");
//     EXPECT_EQ(r.b.cursors[0].point, 5);

//     r = mime::replace(ascii, 0, "b,c", "c,b", 2);
//     EXPECT_EQ(r.n, 1);
//     r = mime::replace(r.b, 0, ",", "#", 2);
//     EXPECT_EQ(r.n, 2);
//     EXPECT_STREQ(mime::to_string(r.b.contents).c_str(), "a,c,b\n1#2#3\n4,5,6\n");
// }

// TEST_F(BufferTest, ReplaceMultiCursor) {
//     auto r = mime::new_cursor(ascii);
//     EXPECT_EQ(r.n, 1);

//     r = mime::replace(r.b, 0, "b,c", "B,c", 0);
//     r = mime::replace(r.b, 1, ",B", ",longfield", 0);
//     EXPECT_EQ(r.n, 1);
//     EXPECT_STREQ(mime::to_string(r.b.contents).c_str(), "a,longfield,c\n1,2,3\n4,5,6\n");
//     EXPECT_EQ(r.b.cursors[0].point, 13);
//     EXPECT_EQ(r.b.cursors[1].point, 11);

//     r = mime::new_cursor(r.b);
//     EXPECT_EQ(r.n, 2);
//     r = mime::new_cursor(r.b);
//     EXPECT_EQ(r.n, 3);
//     r = mime::new_cursor(r.b);
//     EXPECT_EQ(r.n, 4);

//     // move cursor between word "longfield" that is going away.
//     // in this case, cursor 3 should go back to pos 1 where
//     // from field starts.
//     r.b.cursors = r.b.cursors.set(3, mime::cursor{.point = 5});

//     r = mime::replace(r.b, 2, ",longfield", ",www", 0);
//     EXPECT_EQ(r.n, 1);
//     EXPECT_STREQ(mime::to_string(r.b.contents).c_str(), "a,www,c\n1,2,3\n4,5,6\n");
//     EXPECT_EQ(r.b.cursors[0].point, 7);
//     EXPECT_EQ(r.b.cursors[1].point, 5);
//     EXPECT_EQ(r.b.cursors[2].point, 5);
//     EXPECT_EQ(r.b.cursors[3].point, 1);
//     EXPECT_EQ(r.b.cursors[4].point, 0);
// }

// TEST_F(BufferTest, SetMark) {
//     EXPECT_EQ(ascii.cursors[0].mark.has_value(), false);
//     ascii = mime::set_mark(ascii, 0);
//     EXPECT_EQ(ascii.cursors[0].mark.has_value(), true);
//     EXPECT_EQ(ascii.cursors[0].mark.value(), 0);

//     ascii.cursors = ascii.cursors.set(0, mime::cursor{.point = 3});

//     EXPECT_EQ(ascii.cursors[0].mark.has_value(), false);
//     ascii = mime::set_mark(ascii, 0);
//     EXPECT_EQ(ascii.cursors[0].mark.has_value(), true);
//     EXPECT_EQ(ascii.cursors[0].mark.value(), 3);
// }

// TEST_F(BufferTest, CopyEmpty) {
//     EXPECT_EQ(ascii.cursors[0].mark.has_value(), false);
//     auto r = mime::find(ascii, 0, "c"s);
//     auto t = mime::copy(r.b, 0);
//     EXPECT_EQ(t.size(), 0);
//     EXPECT_STREQ(mime::to_string(t).c_str(), "");

//     auto b = mime::set_mark(ascii, 0);
//     t = mime::copy(b, 0);
//     EXPECT_EQ(t.size(), 0);
//     EXPECT_STREQ(mime::to_string(t).c_str(), "");
// }

// TEST_F(BufferTest, CopyForward) {
//     auto b = mime::set_mark(ascii, 0);
//     auto r = mime::find(b, 0, "c"s);
//     auto t = mime::copy(r.b, 0);
//     EXPECT_EQ(t.size(), 5);
//     EXPECT_STREQ(mime::to_string(t).c_str(), "a,b,c");

//     b = mime::forward(r.b, 0, 1);
//     b = mime::set_mark(b, 0);
//     b = mime::end_of_line(b, 0);
//     t = mime::copy(b, 0);
//     EXPECT_EQ(t.size(), 5);
//     EXPECT_STREQ(mime::to_string(t).c_str(), "1,2,3");
// }

// TEST_F(BufferTest, CopyReverse) {
//     auto r = mime::find(ascii, 0, "3"s);
//     auto b = mime::set_mark(r.b, 0);
//     r = mime::rfind(b, 0, "2"s);
//     auto t = mime::copy(r.b, 0);
//     EXPECT_EQ(t.size(), 3);
//     EXPECT_STREQ(mime::to_string(t).c_str(), "2,3");
// }

// TEST_F(BufferTest, EraseRegionEmpty) {
//     auto b = mime::erase_region(ascii, 0);
//     EXPECT_EQ(ascii.contents, b.contents);

//     b = mime::set_mark(b, 0);
//     EXPECT_EQ(ascii.contents, b.contents);
// }

// TEST_F(BufferTest, EraseRegionForward) {
//     auto b = mime::set_mark(ascii, 0);
//     auto i = mime::next_line(b, 0, 1);
//     b = mime::erase_region(i.b, 0);
//     EXPECT_STREQ(mime::to_string(b.contents).c_str(), "1,2,3\n4,5,6\n");

//     auto r = mime::find(b, 0, "3"s);
//     b = mime::set_mark(r.b, 0);
//     i = mime::next_line(b, 0, 2);
//     b = mime::erase_region(i.b, 0);
//     EXPECT_STREQ(mime::to_string(b.contents).c_str(), "1,2,3");
// }

// TEST_F(BufferTest, EraseRegionReverse) {
//     auto r = mime::next_line(ascii, 0, 1);
//     auto b = mime::set_mark(r.b, 0);
//     b = mime::backward(b, 0, 3);
//     b = mime::erase_region(b, 0);
//     EXPECT_STREQ(mime::to_string(b.contents).c_str(), "a,b1,2,3\n4,5,6\n");
// }

// TEST_F(BufferTest, EraseRegionMultiCursor) {
//     // create 4 new cursors
//     auto i = mime::new_cursor(ascii); // go inbetween
//     i = mime::new_cursor(i.b);        // go after
//     i = mime::new_cursor(i.b);        // eraseregion here.

//     auto b = mime::forward(i.b, 1, 3);
//     i = mime::next_line(b, 2, 1);

//     EXPECT_EQ(i.b.cursors[0].point, 0);
//     EXPECT_EQ(i.b.cursors[1].point, 3);
//     EXPECT_EQ(i.b.cursors[2].point, 6);
//     EXPECT_EQ(i.b.cursors[3].point, 0);

//     auto r = mime::find(i.b, 3, ","s);
//     EXPECT_EQ(r.b.cursors[3].point, 2);
//     b = mime::set_mark(r.b, 3);
//     b = mime::end_of_line(b, 3);
//     EXPECT_EQ(b.cursors[3].point, 5);
//     b = mime::erase_region(b, 3);

//     EXPECT_EQ(b.cursors[0].point, 0);
//     EXPECT_EQ(b.cursors[1].point, 2);
//     EXPECT_EQ(b.cursors[2].point, 3);
//     EXPECT_EQ(b.cursors[3].point, 2);
// }

// TEST_F(BufferTest, Cut) {
//     auto b = mime::set_mark(ascii, 0);
//     auto t = mime::cut(b, 0);
//     EXPECT_EQ(t.t.size(), 0);
//     EXPECT_EQ(t.b.contents.size(), 18);

//     auto r = mime::find(b, 0, "c"s);
//     t = mime::cut(r.b, 0);
//     EXPECT_EQ(t.t.size(), 5);
//     EXPECT_STREQ(mime::to_string(t.t).c_str(), "a,b,c");

//     EXPECT_EQ(t.b.contents.size(), 13);
//     EXPECT_STREQ(mime::to_string(t.b.contents).c_str(), "\n1,2,3\n4,5,6\n");
// }

// // TODO: add a MultiCursor test for Paste
// TEST_F(BufferTest, Paste) {
//     auto b = mime::set_mark(ascii, 0);
//     auto r = mime::find(b, 0, "c"s);
//     auto t = mime::copy(r.b, 0);

//     b = mime::paste(r.b, 0, "\n");
//     b = mime::paste(b, 0, t);
//     EXPECT_STREQ(mime::to_string(b.contents).c_str(), "a,b,c\na,b,c\n1,2,3\n4,5,6\n");
// }

// TEST_F(BufferTest, Clear) {
//     auto r = mime::find(ascii, 0, ","s);
//     EXPECT_EQ(r.b.cursors[0].point, 2);

//     auto b = mime::clear(r.b);
//     EXPECT_EQ(b.contents.size(), 0);
//     EXPECT_EQ(b.cursors[0].point, 0);
// }

// TEST_F(BufferTest, GetPos) {
//     EXPECT_EQ(ascii.cursors[0].point, 0);
//     EXPECT_EQ(mime::get_pos(ascii, 0), 0);
//     auto r = mime::find(ascii, 0, ","s);
//     EXPECT_EQ(r.b.cursors[0].point, 2);
//     EXPECT_EQ(mime::get_pos(r.b, 0), 2);
// }

// TEST_F(BufferTest, GotoPos) {
//     auto r = mime::goto_pos(ascii, 0, -1);
//     EXPECT_EQ(r.success, false);

//     r = mime::goto_pos(r.b, 0, 20);
//     EXPECT_EQ(r.success, false);

//     r = mime::goto_pos(r.b, 0, 10);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 10);

//     r = mime::goto_pos(r.b, 0, 5);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 5);

//     r = mime::goto_pos(r.b, 0, 18);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 18);

//     auto b = mime::paste(r.b, 0, "hello");
//     b = mime::start_of_buffer(b, 0);
//     r = mime::find(b, 0, "hel"s);
//     EXPECT_EQ(r.b.cursors[0].point, 21);
//     EXPECT_EQ(r.success, true);
// }

// TEST_F(BufferTest, Forward) {
//     auto b = mime::forward(ascii, 0, 1);
//     EXPECT_EQ(b.cursors[0].point, 1);

//     b = mime::forward(b, 0, 1);
//     EXPECT_EQ(b.cursors[0].point, 2);

//     b = mime::forward(b, 0, 16);
//     EXPECT_EQ(b.cursors[0].point, 18);

//     b = mime::forward(ascii, 0, 20);
//     EXPECT_EQ(b.cursors[0].point, 18);
// }

// TEST_F(BufferTest, Backward) {
//     auto b = mime::forward(ascii, 0, 18);
//     EXPECT_EQ(b.cursors[0].point, 18);

//     b = mime::backward(b, 0, 1);
//     EXPECT_EQ(b.cursors[0].point, 17);
//     b = mime::backward(b, 0, 5);
//     EXPECT_EQ(b.cursors[0].point, 12);

//     b = mime::backward(b, 0, 12);
//     EXPECT_EQ(b.cursors[0].point, 0);

//     b = mime::backward(b, 0, 20);
//     EXPECT_EQ(b.cursors[0].point, 0);
// }

// TEST_F(BufferTest, NextLine) {
//     auto i = mime::next_line(ascii, 0, 1);
//     EXPECT_EQ(i.b.cursors[0].point, 6);
//     EXPECT_EQ(i.n, 1);

//     i = mime::next_line(i.b, 0, 1);
//     EXPECT_EQ(i.b.cursors[0].point, 12);
//     EXPECT_EQ(i.n, 1);

//     i = mime::next_line(i.b, 0, 2);
//     EXPECT_EQ(i.b.cursors[0].point, 18);
//     EXPECT_EQ(i.n, 1);
// }

// TEST_F(BufferTest, PrevLine) {
//     auto r = mime::goto_pos(ascii, 0, 18);
//     auto i = mime::prev_line(r.b, 0, 1);
//     EXPECT_EQ(i.b.cursors[0].point, 12);
//     EXPECT_EQ(i.n, 1);

//     i = mime::prev_line(i.b, 0, 1);
//     EXPECT_EQ(i.b.cursors[0].point, 6);
//     EXPECT_EQ(i.n, 1);

//     i = mime::prev_line(i.b, 0, 2);
//     EXPECT_EQ(i.b.cursors[0].point, 0);
//     EXPECT_EQ(i.n, 1);
// }

// TEST_F(BufferTest, StartOfBuffer) {
//     auto r = mime::goto_pos(ascii, 0, 12);
//     EXPECT_EQ(r.b.cursors[0].point, 12);

//     auto b = mime::start_of_buffer(r.b, 0);
//     EXPECT_EQ(b.cursors[0].point, 0);
// }

// TEST_F(BufferTest, EndOfBuffer) {
//     auto r = mime::goto_pos(ascii, 0, 12);
//     EXPECT_EQ(r.b.cursors[0].point, 12);

//     auto b = mime::end_of_buffer(r.b, 0);
//     EXPECT_EQ(b.cursors[0].point, 18);
// }

// TEST_F(BufferTest, StartOfLine) {
//     auto r = mime::goto_pos(ascii, 0, 15);
//     EXPECT_EQ(r.b.cursors[0].point, 15);

//     auto b = mime::start_of_line(r.b, 0);
//     EXPECT_EQ(b.cursors[0].point, 12);
// }

// TEST_F(BufferTest, EndOfLine) {
//     auto r = mime::goto_pos(ascii, 0, 12);
//     EXPECT_EQ(r.b.cursors[0].point, 12);

//     auto b = mime::end_of_line(r.b, 0);
//     EXPECT_EQ(b.cursors[0].point, 17);
// }

// // TODO:  convert below tests to fixture based ones.
// TEST(BufferOpen, UnnamedFile) {
//     auto r = mime::open("");
//     EXPECT_EQ(r.get_bool(), true); // signifying a new file.
//     EXPECT_EQ(r.get_buffer().contents.size(), 0);
// }

// TEST(BufferOpen, ExistingFile) {
//     auto r = mime::open("testdata/open.csv");
//     EXPECT_EQ(r.get_bool(), false);
//     EXPECT_EQ(r.get_buffer().contents.size(), 18);
// }

// TEST(BufferOpen, NewFile) {
//     auto r = mime::open("testdata/newfile");
//     EXPECT_EQ(r.get_bool(), true);
//     EXPECT_EQ(r.get_buffer().contents.size(), 0);
// }

// TEST(BufferSave, NewFile) {
//     auto r = mime::open("testdata/newfile");
//     EXPECT_EQ(r.get_bool(), true);
//     r.b = mime::paste(r.b, 0, "hello");
//     save(r.b);
//     r = mime::open("testdata/newfile");
//     EXPECT_EQ(r.get_bool(), false);
//     EXPECT_EQ(r.get_buffer().contents.size(), 5);
//     std::filesystem::remove("testdata/newfile");
// }

// TEST(BufferFind, Present) {
//     auto r = mime::open("testdata/open.csv");

//     r = mime::find(r.b, 0, "a"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 1);

//     r = mime::find(r.b, 0, "b"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 3);

//     r = mime::find(r.b, 0, "c\n1"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 7);

//     r = mime::find(r.b, 0, "6\n"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 18);
// }

// TEST(BufferFind, Missing) {
//     auto r = mime::open("testdata/open.csv");

//     r = mime::find(r.b, 0, "zz"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 0);

//     r = mime::find(r.b, 0, "b"s);
//     EXPECT_EQ(r.b.cursors[0].point, 3);

//     r = mime::find(r.b, 0, "zzz"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 3);

//     r = mime::find(r.b, 0, ""s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 3);

//     r = mime::find(r.b, 0, "6\n"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 18);

//     r = mime::find(r.b, 0, "6\n"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 18);
// }

// TEST(BufferFind, EmptyFile) {
//     auto r = mime::open("");
//     EXPECT_EQ(r.b.cursors[0].point, 0);
//     r = mime::find(r.b, 0, "zz"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 0);
// }

// TEST(BufferRfind, Present) {
//     auto r = mime::open("testdata/open.csv");

//     r = mime::find(r.b, 0, "6\n"s);
//     EXPECT_EQ(r.b.cursors[0].point, 18);

//     r = mime::rfind(r.b, 0, "3"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 10);

//     r = mime::rfind(r.b, 0, "a,b"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 0);
// }

// TEST(BufferRfind, Missing) {
//     auto r = mime::open("testdata/open.csv");

//     // TODO: make this work
//     // r = mime::rfind(r.b, 0, "a,b"s, 0);
//     // EXPECT_EQ(r.success, false);
//     // EXPECT_EQ(r.b.cursors[0].point, 18);

//     r = mime::find(r.b, 0, "6\n"s);
//     EXPECT_EQ(r.b.cursors[0].point, 18);

//     r = mime::rfind(r.b, 0, ""s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 18);

//     r = mime::rfind(r.b, 0, "d"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 18);

//     r = mime::rfind(r.b, 0, "a,b,d"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 18);
// }

// TEST(BufferFindFuzzy, Present) {
//     auto r = mime::open("testdata/open.csv");

//     r = mime::find_fuzzy(r.b, 0, "a,B,C 1,2"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 9);

//     r = mime::find_fuzzy(r.b, 0, "3\n\n\t4"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 13);

//     // TODO: make this work
//     // r = mime::find_fuzzy(r.b, 0, "5, 6"s, 0);
//     // EXPECT_EQ(r.success, true);
//     // EXPECT_EQ(r.b.cursors[0].point, 13);
// }

// TEST(BufferFindFuzzy, Missing) {
//     auto r = mime::open("testdata/open.csv");

//     r = mime::find_fuzzy(r.b, 0, "a,B,d 1,2"s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 0);

//     r = mime::find_fuzzy(r.b, 0, ""s);
//     EXPECT_EQ(r.success, false);
//     EXPECT_EQ(r.b.cursors[0].point, 0);
// }

// TEST(BufferFindFuzzy, Unicode) {
//     auto r = mime::open("testdata/unicode.csv");

//     r = mime::find_fuzzy(r.b, 0, "நாமன் உன்"s);
//     EXPECT_EQ(r.success, true);
//     EXPECT_EQ(r.b.cursors[0].point, 38);
// }
