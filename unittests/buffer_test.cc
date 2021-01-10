#include <filesystem>
#include <gtest/gtest.h>
#include <mime/mime.hh>
#include <string>

using namespace std::literals;

class BufferTest : public ::testing::Test {
  protected:
    void SetUp() override {
        std::string aname{"testdata/open.csv"};
        ascii = mime::buffer(mime::text{aname.begin(), aname.end()});
        unicode = mime::buffer("testdata/unicode.csv");
    }

    mime::buffer ascii;
    mime::buffer unicode;
};

TEST_F(BufferTest, ReplaceNotFound) {
    auto n = ascii.replace("", "c,b", 0);
    EXPECT_EQ(n, 0);

    n = ascii.replace("zzwwd", "c,b", 0);
    EXPECT_EQ(n, 0);
}

TEST_F(BufferTest, ReplaceFound) {
    auto r = ascii;
    auto n = r.replace("b,c", "c,b", 0);
    EXPECT_EQ(n, 1);
    EXPECT_STREQ(mime::to_string(r.get_contents()).c_str(), "a,c,b\n1,2,3\n4,5,6\n");
    EXPECT_EQ(r.get_pos(), 5);

    n = ascii.replace("b,c", "c,b", 2);
    EXPECT_EQ(n, 1);
    n = ascii.replace(",", "#", 2);
    EXPECT_EQ(n, 2);
    EXPECT_STREQ(mime::to_string(ascii.get_contents()).c_str(), "a,c,b\n1#2#3\n4,5,6\n");
}

TEST_F(BufferTest, ReplaceMultiCursor) {
    auto n = ascii.new_cursor();
    EXPECT_EQ(n, 1);

    auto r = ascii;
    r.replace("b,c", "B,c", 0);
    r.use_cursor(1);
    n = r.replace(",B", ",longfield", 0);
    EXPECT_EQ(n, 1);
    EXPECT_STREQ(mime::to_string(r.get_contents()).c_str(), "a,longfield,c\n1,2,3\n4,5,6\n");
    r.use_cursor(0);
    EXPECT_EQ(r.get_pos(), 13);
    r.use_cursor(1);
    EXPECT_EQ(r.get_pos(), 11);

    r.new_cursor();
    r.new_cursor();
    r.new_cursor();

    // move cursor between word "longfield" that is going away.
    // in this case, cursor 3 should go back to pos 1 where
    // from field starts.
    r.use_cursor(3);
    r.goto_pos(5);

    r.use_cursor(2);
    n = r.replace(",longfield", ",www", 0);
    EXPECT_EQ(n, 1);
    EXPECT_STREQ(mime::to_string(r.get_contents()).c_str(), "a,www,c\n1,2,3\n4,5,6\n");

    r.use_cursor(0);
    EXPECT_EQ(r.get_pos(), 7);
    r.use_cursor(1);
    EXPECT_EQ(r.get_pos(), 5);
    r.use_cursor(2);
    EXPECT_EQ(r.get_pos(), 5);
    r.use_cursor(3);
    EXPECT_EQ(r.get_pos(), 1);
    r.use_cursor(4);
    EXPECT_EQ(r.get_pos(), 0);
}

// TODO: below test still need to be converted to new api.

TEST_F(BufferTest, SetMark) {
    EXPECT_EQ(ascii.get_mark(), -1);
    ascii.set_mark();
    EXPECT_EQ(ascii.get_mark(), 0);

    ascii.goto_pos(3);

    ascii.set_mark();
    EXPECT_EQ(ascii.get_mark(), 3);
}

TEST_F(BufferTest, CopyEmpty) {
    EXPECT_EQ(ascii.get_mark(), -1);
    ascii.find("c"s);
    auto t = ascii.copy();
    EXPECT_EQ(t.size(), 0);
    EXPECT_STREQ(mime::to_string(t).c_str(), "");

    ascii.set_mark();
    t = ascii.copy();
    EXPECT_EQ(t.size(), 0);
    EXPECT_STREQ(mime::to_string(t).c_str(), "");
}

TEST_F(BufferTest, CopyForward) {
    ascii.set_mark();
    ascii.find("c"s);
    auto t = ascii.copy();
    EXPECT_EQ(t.size(), 5);
    EXPECT_STREQ(mime::to_string(t).c_str(), "a,b,c");

    ascii.forward();
    ascii.set_mark();
    ascii.end_of_line();
    t = ascii.copy();
    EXPECT_EQ(t.size(), 5);
    EXPECT_STREQ(mime::to_string(t).c_str(), "1,2,3");
}

TEST_F(BufferTest, CopyReverse) {
    ascii.find("3"s);
    ascii.set_mark();
    ascii.rfind("2"s);
    auto t = ascii.copy();
    EXPECT_EQ(t.size(), 3);
    EXPECT_STREQ(mime::to_string(t).c_str(), "2,3");
}

TEST_F(BufferTest, EraseRegionEmpty) {
    auto r = ascii;
    ascii.erase_region();
    EXPECT_EQ(ascii.get_contents(), r.get_contents());

    ascii.set_mark();
    ascii.erase_region();
    EXPECT_EQ(ascii.get_contents(), r.get_contents());
}

TEST_F(BufferTest, EraseRegionForward) {
    ascii.set_mark();
    ascii.next_line();
    ascii.erase_region();
    EXPECT_STREQ(mime::to_string(ascii.get_contents()).c_str(), "1,2,3\n4,5,6\n");

    ascii.find("3"s);
    ascii.set_mark();
    ascii.next_line(2);
    ascii.erase_region();
    EXPECT_STREQ(mime::to_string(ascii.get_contents()).c_str(), "1,2,3");
}

TEST_F(BufferTest, EraseRegionReverse) {
    ascii.next_line();
    ascii.set_mark();
    ascii.backward(3);
    ascii.erase_region();
    EXPECT_STREQ(mime::to_string(ascii.get_contents()).c_str(), "a,b1,2,3\n4,5,6\n");
}

TEST_F(BufferTest, EraseRegionMultiCursor) {
    // create 4 new cursors
    ascii.new_cursor();
    ascii.new_cursor();
    ascii.new_cursor();

    ascii.use_cursor(1);
    ascii.forward(3);
    ascii.use_cursor(2);
    ascii.next_line();

    ascii.use_cursor(0);
    EXPECT_EQ(ascii.get_pos(), 0);
    ascii.use_cursor(1);
    EXPECT_EQ(ascii.get_pos(), 3);
    ascii.use_cursor(2);
    EXPECT_EQ(ascii.get_pos(), 6);
    ascii.use_cursor(3);
    EXPECT_EQ(ascii.get_pos(), 0);

    ascii.find(","s);
    EXPECT_EQ(ascii.get_pos(), 2);
    ascii.set_mark();
    ascii.end_of_line();
    EXPECT_EQ(ascii.get_pos(), 5);
    ascii.erase_region();

    ascii.use_cursor(0);
    EXPECT_EQ(ascii.get_pos(), 0);
    ascii.use_cursor(1);
    EXPECT_EQ(ascii.get_pos(), 2);
    ascii.use_cursor(2);
    EXPECT_EQ(ascii.get_pos(), 3);
    ascii.use_cursor(3);
    EXPECT_EQ(ascii.get_pos(), 2);
}

TEST_F(BufferTest, Cut) {
    ascii.set_mark();
    auto t = ascii.cut();
    EXPECT_EQ(t.size(), 0);
    EXPECT_EQ(ascii.size(), 18);

    ascii.find("c"s);
    t = ascii.cut();
    EXPECT_EQ(t.size(), 5);
    EXPECT_STREQ(mime::to_string(t).c_str(), "a,b,c");

    EXPECT_EQ(ascii.size(), 13);
    EXPECT_STREQ(mime::to_string(ascii.get_contents()).c_str(), "\n1,2,3\n4,5,6\n");
}

// // TODO: add a MultiCursor test for Paste
TEST_F(BufferTest, Paste) {
    ascii.set_mark();
    ascii.find("c"s);
    auto t = ascii.copy();

    ascii.paste("\n");
    ascii.paste(t);
    EXPECT_STREQ(mime::to_string(ascii.get_contents()).c_str(), "a,b,c\na,b,c\n1,2,3\n4,5,6\n");
}

TEST_F(BufferTest, Clear) {
    ascii.find(","s);
    EXPECT_EQ(ascii.get_pos(), 2);

    ascii.clear();
    EXPECT_EQ(ascii.size(), 0);
    EXPECT_EQ(ascii.get_pos(), 0);
}

TEST_F(BufferTest, GetPos) {
    EXPECT_EQ(ascii.get_pos(), 0);
    ascii.find(","s);
    EXPECT_EQ(ascii.get_pos(), 2);
}

TEST_F(BufferTest, GotoPos) {
    auto b = ascii.goto_pos(-1);
    EXPECT_EQ(b, false);

    b = ascii.goto_pos(20);
    EXPECT_EQ(b, false);

    b = ascii.goto_pos(10);
    EXPECT_EQ(b, true);
    EXPECT_EQ(ascii.get_pos(), 10);

    b = ascii.goto_pos(5);
    EXPECT_EQ(b, true);
    EXPECT_EQ(ascii.get_pos(), 5);

    b = ascii.goto_pos(18);
    EXPECT_EQ(b, true);
    EXPECT_EQ(ascii.get_pos(), 18);

    b = ascii.goto_pos(-1);
    EXPECT_EQ(b, false);
    EXPECT_EQ(ascii.get_pos(), 18);

    ascii.paste("hello");
    ascii.start_of_buffer();
    auto r = ascii.find("hel"s);
    EXPECT_EQ(ascii.get_pos(), 21);
    EXPECT_EQ(r, 18);
}

TEST_F(BufferTest, Forward) {
    ascii.forward();
    EXPECT_EQ(ascii.get_pos(), 1);

    ascii.forward();
    EXPECT_EQ(ascii.get_pos(), 2);

    ascii.forward(16);
    EXPECT_EQ(ascii.get_pos(), 18);

    ascii.forward(20);
    EXPECT_EQ(ascii.get_pos(), 18);
}

TEST_F(BufferTest, Backward) {
    ascii.forward(18);
    EXPECT_EQ(ascii.get_pos(), 18);

    ascii.backward(1);
    EXPECT_EQ(ascii.get_pos(), 17);
    ascii.backward(5);
    EXPECT_EQ(ascii.get_pos(), 12);
    ascii.backward(12);
    EXPECT_EQ(ascii.get_pos(), 0);

    ascii.backward(20);
    EXPECT_EQ(ascii.get_pos(), 0);
}

TEST_F(BufferTest, NextLine) {
    auto n = ascii.next_line();
    EXPECT_EQ(ascii.get_pos(), 6);
    EXPECT_EQ(n, 1);

    n = ascii.next_line();
    EXPECT_EQ(ascii.get_pos(), 12);
    EXPECT_EQ(n, 1);

    n = ascii.next_line(4);
    EXPECT_EQ(ascii.get_pos(), 18);
    EXPECT_EQ(n, 1);
}

TEST_F(BufferTest, PrevLine) {
    ascii.goto_pos(18);
    auto n = ascii.prev_line();
    EXPECT_EQ(ascii.get_pos(), 12);
    EXPECT_EQ(n, 1);

    n = ascii.prev_line();
    EXPECT_EQ(ascii.get_pos(), 6);
    EXPECT_EQ(n, 1);

    n = ascii.prev_line(2);
    EXPECT_EQ(ascii.get_pos(), 0);
    EXPECT_EQ(n, 1);
}

TEST_F(BufferTest, StartOfBuffer) {
    ascii.goto_pos(12);
    EXPECT_EQ(ascii.get_pos(), 12);
    ascii.start_of_buffer();
    EXPECT_EQ(ascii.get_pos(), 0);
}

TEST_F(BufferTest, EndOfBuffer) {
    ascii.goto_pos(12);
    EXPECT_EQ(ascii.get_pos(), 12);
    ascii.end_of_buffer();
    EXPECT_EQ(ascii.get_pos(), 18);
}

TEST_F(BufferTest, StartOfLine) {
    ascii.goto_pos(15);
    EXPECT_EQ(ascii.get_pos(), 15);

    ascii.start_of_line();
    EXPECT_EQ(ascii.get_pos(), 12);
}

TEST_F(BufferTest, EndOfLine) {
    ascii.goto_pos(12);
    EXPECT_EQ(ascii.get_pos(), 12);

    ascii.end_of_line();
    EXPECT_EQ(ascii.get_pos(), 17);
}

TEST_F(BufferTest, Open) {
    auto b = mime::buffer();
    EXPECT_EQ(b.size(), 0);
    b = mime::buffer("/non/existant");
    EXPECT_EQ(b.get_name(), "/non/existant");
    EXPECT_EQ(b.size(), 0);
    EXPECT_EQ(ascii.size(), 18);
}

TEST_F(BufferTest, Save) {
    auto b = mime::buffer("testdata/newfile");
    b.paste("hello");
    b.save();
    auto r = mime::buffer("testdata/newfile");
    EXPECT_EQ(r.size(), 5);
    std::filesystem::remove("testdata/newfile");
}

TEST_F(BufferTest, FindPresent) {
    auto r = ascii.find("a"s);
    EXPECT_EQ(r, 0);
    EXPECT_EQ(ascii.get_pos(), 1);

    r = ascii.find("b"s);
    EXPECT_EQ(r, 2);
    EXPECT_EQ(ascii.get_pos(), 3);

    r = ascii.find("c\n1"s);
    EXPECT_EQ(r, 4);
    EXPECT_EQ(ascii.get_pos(), 7);

    r = ascii.find("6\n"s);
    EXPECT_EQ(r, 16);
    EXPECT_EQ(ascii.get_pos(), 18);
}

TEST_F(BufferTest, FindMissing) {
    auto r = ascii.find("zz"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 0);

    r = ascii.find("b"s);
    EXPECT_EQ(r, 2);
    EXPECT_EQ(ascii.get_pos(), 3);

    r = ascii.find("zzz"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 3);

    r = ascii.find(""s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 3);

    r = ascii.find("6\n"s);
    EXPECT_EQ(r, 16);
    EXPECT_EQ(ascii.get_pos(), 18);

    r = ascii.find("6\n"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 18);
}

TEST_F(BufferTest, FindEmptyFile) {
    auto b = mime::buffer();
    EXPECT_EQ(b.get_pos(), 0);
    auto r = b.find("zz"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(b.get_pos(), 0);
}

TEST_F(BufferTest, FindRegex) {
    auto r = ascii.find(mime::regex("[0-9]"));
    EXPECT_EQ(r, 6);
    EXPECT_EQ(ascii.get_pos(), 7);

    r = ascii.find(mime::regex(""));
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 7);
}

TEST_F(BufferTest, RFindPresent) {
    ascii.find("6\n"s);
    EXPECT_EQ(ascii.get_pos(), 18);

    auto r = ascii.rfind("3"s);
    EXPECT_EQ(r, 11);
    EXPECT_EQ(ascii.get_pos(), 10);

    r = ascii.rfind("a,b"s);
    EXPECT_EQ(r, 3);
    EXPECT_EQ(ascii.get_pos(), 0);
}

TEST_F(BufferTest, RFindMissing) {

    // TODO: make this work
    // r = mime::rfind(r.b, 0, "a,b"s, 0);
    // EXPECT_EQ(r.success, false);
    // EXPECT_EQ(r.b.cursors[0].point, 18);

    ascii.find("6\n"s);
    EXPECT_EQ(ascii.get_pos(), 18);

    auto r = ascii.rfind(""s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 18);

    r = ascii.rfind("d"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 18);

    r = ascii.rfind("a,b,d"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 18);
}

TEST_F(BufferTest, FindFuzzyPresent) {
    auto r = ascii.find_fuzzy("a,B,C 1,2"s);
    EXPECT_EQ(r, 0);
    EXPECT_EQ(ascii.get_pos(), 9);

    r = ascii.find_fuzzy("3\n\n\t4"s);
    EXPECT_EQ(r, 10);
    EXPECT_EQ(ascii.get_pos(), 13);

    // TODO: make this work
    // r = mime::find_fuzzy(r.b, 0, "5, 6"s, 0);
    // EXPECT_EQ(r.success, true);
    // EXPECT_EQ(r.b.cursors[0].point, 13);
}

TEST_F(BufferTest, FindFuzzyMissing) {
    auto r = ascii.find_fuzzy("a,B,d 1,2"s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 0);

    r = ascii.find_fuzzy(""s);
    EXPECT_LT(r, 0);
    EXPECT_EQ(ascii.get_pos(), 0);
}

TEST_F(BufferTest, FindFuzzyUnicode) {
    auto r = unicode.find_fuzzy("நாமன் உன்"s);
    EXPECT_EQ(r, 29);
    EXPECT_EQ(unicode.get_pos(), 38);
}
