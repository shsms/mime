#include <gtest/gtest.h>
#include <mime/mime.hh>
#include <string>

using namespace std::literals;

class BlockTest : public ::testing::Test {
  protected:
    void SetUp() override { gosrc = mime::buffer("testdata/narrow_test_input.go"); }

    mime::buffer gosrc;
};

TEST_F(BlockTest, StartOfBlock) {
    EXPECT_EQ(gosrc.start_of_block(), false);
    EXPECT_EQ(gosrc.get_pos(), 0);

    gosrc.find("world"s);
    EXPECT_EQ(gosrc.start_of_block(), true);
    EXPECT_EQ(gosrc.get_pos(), 40);

    gosrc.find("Println"s);
    EXPECT_EQ(gosrc.start_of_block(), true);
    EXPECT_EQ(gosrc.get_pos(), 113);
}

TEST_F(BlockTest, EndOfBlock) {
    EXPECT_EQ(gosrc.end_of_block(), false);
    EXPECT_EQ(gosrc.get_pos(), 0);

    gosrc.find("world"s);
    EXPECT_EQ(gosrc.end_of_block(), true);
    EXPECT_EQ(gosrc.get_pos(), 159);

    gosrc.rfind("Println"s);
    EXPECT_EQ(gosrc.end_of_block(), true);
    EXPECT_EQ(gosrc.get_pos(), 133);
}

class NarrowTest : public ::testing::Test {
  protected:
    void SetUp() override {
        gosrc = mime::buffer("testdata/narrow_test_input.go");

        narrowed = gosrc;
        narrowed.find("{"s);
        narrowed.narrow_to_block();
    }

    mime::buffer gosrc;
    mime::buffer narrowed;
};

TEST_F(NarrowTest, EmptySize) {
    EXPECT_EQ(gosrc.size(), 244);
    EXPECT_EQ(narrowed.size(), 119);

    EXPECT_EQ(gosrc.empty(), false);
    EXPECT_EQ(narrowed.empty(), false);

    narrowed.start_of_buffer();
    narrowed.set_mark();
    narrowed.end_of_buffer();
    narrowed.erase_region();

    EXPECT_EQ(narrowed.empty(), true);
    EXPECT_EQ(narrowed.size(), 0);

    narrowed.widen();

    EXPECT_EQ(narrowed.empty(), false);
    EXPECT_EQ(narrowed.size(), 125);
}

TEST_F(NarrowTest, NarrowToBlock) {
    EXPECT_EQ(gosrc.narrow_to_block(), false);
    EXPECT_EQ(gosrc.get_pos(), 0);
    EXPECT_EQ(gosrc.narrowed(), false);

    EXPECT_EQ(gosrc.find("world"s), 91);
    EXPECT_EQ(gosrc.narrow_to_block(), true);
    EXPECT_EQ(gosrc.narrowed(), true);
    EXPECT_EQ(gosrc.get_pos(), 40);
    EXPECT_EQ(gosrc.find("world"s), 91);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 40);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 159);

    EXPECT_EQ(gosrc.rfind("hello"s), 71);
    EXPECT_EQ(gosrc.narrow_to_block(), true);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 52);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 77);
}

TEST_F(NarrowTest, Widen) {
    narrowed.start_of_buffer();
    EXPECT_EQ(narrowed.get_pos(), 40);
    narrowed.end_of_buffer();
    EXPECT_EQ(narrowed.get_pos(), 159);
    EXPECT_EQ(narrowed.narrowed(), true);

    narrowed.widen();
    narrowed.start_of_buffer();
    EXPECT_EQ(narrowed.get_pos(), 0);
    narrowed.end_of_buffer();
    EXPECT_EQ(narrowed.get_pos(), 244);
    EXPECT_EQ(narrowed.narrowed(), false);
}

TEST_F(NarrowTest, Find) {
    EXPECT_EQ(narrowed.get_pos(), 40);
    EXPECT_EQ(narrowed.find("else {"s), 134);
    EXPECT_EQ(narrowed.get_pos(), 140);
    EXPECT_EQ(narrowed.find("return"s), -1);
    EXPECT_EQ(narrowed.get_pos(), 140);
}

TEST_F(NarrowTest, FindRegex) {
    EXPECT_EQ(narrowed.get_pos(), 40);
    EXPECT_EQ(narrowed.find(mime::regex("[a-z]+d\\(")), 91);
    EXPECT_EQ(narrowed.get_pos(), 97);
    EXPECT_EQ(narrowed.find(mime::regex("[a-z]+d\\(")), -1);
    EXPECT_EQ(narrowed.get_pos(), 97);

    EXPECT_EQ(gosrc.find(mime::regex("[a-z]+d\\(")), 91);
    EXPECT_EQ(gosrc.get_pos(), 97);
    EXPECT_EQ(gosrc.find(mime::regex("[a-z]+d\\(")), 166);
    EXPECT_EQ(gosrc.get_pos(), 172);
}

TEST_F(NarrowTest, RFind) {
    EXPECT_EQ(narrowed.get_pos(), 40);
    EXPECT_EQ(narrowed.rfind("fmt"s), -1);
    narrowed.end_of_buffer();
    EXPECT_EQ(narrowed.get_pos(), 159);
    EXPECT_EQ(narrowed.rfind("fmt"s), 146);
    EXPECT_EQ(narrowed.get_pos(), 143);

    EXPECT_EQ(gosrc.goto_pos(40), true);
    EXPECT_EQ(gosrc.get_pos(), 40);
    EXPECT_EQ(gosrc.rfind("fmt"s), 25);
    EXPECT_EQ(gosrc.get_pos(), 22);
}

TEST_F(NarrowTest, FindFuzzy) {
    EXPECT_EQ(narrowed.get_pos(), 40);
    EXPECT_EQ(narrowed.find_fuzzy("else {"s), 134);
    EXPECT_EQ(narrowed.get_pos(), 140);
    EXPECT_EQ(narrowed.find_fuzzy("return"s), -1);
    EXPECT_EQ(narrowed.get_pos(), 140);
}

TEST_F(NarrowTest, PrevLine) {
    EXPECT_EQ(narrowed.get_pos(), 40);
    EXPECT_EQ(narrowed.next_line(), 1);
    EXPECT_EQ(narrowed.get_pos(), 42);

    EXPECT_EQ(narrowed.prev_line(2), 1);
    EXPECT_EQ(narrowed.get_pos(), 40);
}

TEST_F(NarrowTest, NextLine) {
    narrowed.end_of_buffer();
    EXPECT_EQ(narrowed.get_pos(), 159);
    EXPECT_EQ(narrowed.prev_line(), 1);
    EXPECT_EQ(narrowed.get_pos(), 155);

    EXPECT_EQ(narrowed.next_line(2), 1);
    EXPECT_EQ(narrowed.get_pos(), 159);
}

TEST_F(NarrowTest, StartOfLine) {
    narrowed.end_of_line();
    EXPECT_EQ(narrowed.get_pos(), 41);
    narrowed.start_of_line();
    EXPECT_EQ(narrowed.get_pos(), 40);
}

TEST_F(NarrowTest, EndOfLine) {
    auto cur = narrowed.new_cursor();
    narrowed.use_cursor(cur);
    narrowed.set_mark();
    narrowed.find("world"s);
    EXPECT_EQ(narrowed.get_pos(), 96);
    narrowed.narrow_to_region();

    narrowed.end_of_buffer();
    narrowed.start_of_line();
    EXPECT_EQ(narrowed.get_pos(), 80);
    narrowed.end_of_line();
    EXPECT_EQ(narrowed.get_pos(), 96);
}

class MultiCursorNarrowTest : public ::testing::Test {
  protected:
    void SetUp() override {
        gosrc = mime::buffer("testdata/narrow_test_input.go");
        full_c = gosrc.new_cursor();

        main_c = gosrc.new_cursor();
        gosrc.use_cursor(main_c);
        gosrc.find("{"s);
        gosrc.narrow_to_block();

        world_c = gosrc.new_cursor();
        gosrc.use_cursor(world_c);
        auto wbeg = gosrc.find("world"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        if_c = gosrc.new_cursor();
        gosrc.use_cursor(if_c);
        gosrc.find("if(true) {"s);
        gosrc.narrow_to_block();

        wor_c = gosrc.new_cursor();
        gosrc.use_cursor(wor_c);
        wbeg = gosrc.find("wor"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        orl_c = gosrc.new_cursor();
        gosrc.use_cursor(orl_c);
        wbeg = gosrc.find("orl"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        rld_c = gosrc.new_cursor();
        gosrc.use_cursor(rld_c);
        wbeg = gosrc.find("rld"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        rld_eol_c = gosrc.new_cursor();
        gosrc.use_cursor(rld_eol_c);
        wbeg = gosrc.find("rld())"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        print_c = gosrc.new_cursor();
        gosrc.use_cursor(print_c);
        gosrc.find("hello"s);
        wbeg = gosrc.find("Print("s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        print_wor_c = gosrc.new_cursor();
        gosrc.use_cursor(print_wor_c);
        wbeg = gosrc.find("Print(wor"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();

        func_world_c = gosrc.new_cursor();
        gosrc.use_cursor(func_world_c);
        wbeg = gosrc.find("func world"s);
        gosrc.set_mark();
        gosrc.goto_pos(wbeg);
        gosrc.narrow_to_region();
    }

    mime::buffer gosrc;

    std::size_t full_c{};
    std::size_t main_c{};
    std::size_t world_c{};
    std::size_t wor_c{};
    std::size_t orl_c{};
    std::size_t rld_c{};
    std::size_t rld_eol_c{};
    std::size_t print_c{};
    std::size_t print_wor_c{};
    std::size_t func_world_c{};
    std::size_t if_c{};
};

TEST_F(MultiCursorNarrowTest, CheckCursors) {
    gosrc.use_cursor(full_c);
    EXPECT_EQ(gosrc.get_pos(), 0);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 244);

    gosrc.use_cursor(main_c);
    EXPECT_EQ(gosrc.get_pos(), 40);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 159);

    gosrc.use_cursor(world_c);
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 96);

    gosrc.use_cursor(wor_c);
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 94);

    gosrc.use_cursor(orl_c);
    EXPECT_EQ(gosrc.get_pos(), 92);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 95);

    gosrc.use_cursor(rld_c);
    EXPECT_EQ(gosrc.get_pos(), 93);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 96);

    gosrc.use_cursor(rld_eol_c);
    EXPECT_EQ(gosrc.get_pos(), 93);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 99);

    gosrc.use_cursor(print_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(print_wor_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 94);

    gosrc.use_cursor(func_world_c);
    EXPECT_EQ(gosrc.get_pos(), 161);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 171);

    gosrc.use_cursor(if_c);
    EXPECT_EQ(gosrc.get_pos(), 52);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 77);
}

TEST_F(MultiCursorNarrowTest, EraseRegion) {
    gosrc.use_cursor(world_c);
    gosrc.set_mark();
    gosrc.end_of_buffer();
    gosrc.erase_region();

    EXPECT_TRUE(gosrc.get_contents().empty());

    gosrc.use_cursor(full_c);
    EXPECT_EQ(gosrc.get_pos(), 0);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 239);

    gosrc.use_cursor(main_c);
    EXPECT_EQ(gosrc.get_pos(), 40);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 154);

    gosrc.use_cursor(world_c);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(wor_c);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(orl_c);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(rld_c);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(rld_eol_c);
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 94);

    gosrc.use_cursor(print_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(print_wor_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(func_world_c);
    EXPECT_EQ(gosrc.get_pos(), 156);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 166);

    gosrc.use_cursor(if_c);
    EXPECT_EQ(gosrc.get_pos(), 52);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 77);
}

TEST_F(MultiCursorNarrowTest, Paste) {
    gosrc.use_cursor(world_c);
    gosrc.paste("new ");

    EXPECT_EQ(mime::to_string(gosrc.get_contents()), "new world");

    gosrc.use_cursor(full_c);
    EXPECT_EQ(gosrc.get_pos(), 0);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 248);

    gosrc.use_cursor(main_c);
    EXPECT_EQ(gosrc.get_pos(), 40);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 163);

    gosrc.use_cursor(world_c);
    EXPECT_EQ(gosrc.get_pos(), 95);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 100);

    gosrc.use_cursor(wor_c);
    EXPECT_EQ(gosrc.get_pos(), 95);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 98);

    gosrc.use_cursor(orl_c);
    EXPECT_EQ(gosrc.get_pos(), 96);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 99);

    gosrc.use_cursor(rld_c);
    EXPECT_EQ(gosrc.get_pos(), 97);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 100);

    gosrc.use_cursor(rld_eol_c);
    EXPECT_EQ(gosrc.get_pos(), 97);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 103);

    gosrc.use_cursor(print_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 95);

    gosrc.use_cursor(print_wor_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 98);

    gosrc.use_cursor(func_world_c);
    EXPECT_EQ(gosrc.get_pos(), 165);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 175);

    gosrc.use_cursor(if_c);
    EXPECT_EQ(gosrc.get_pos(), 52);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 77);
}

TEST_F(MultiCursorNarrowTest, Replace) {
    gosrc.use_cursor(world_c);
    gosrc.replace("world", "neue welt");

    EXPECT_EQ(mime::to_string(gosrc.get_contents()), "neue welt");

    gosrc.use_cursor(full_c);
    EXPECT_EQ(gosrc.get_pos(), 0);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 248);

    gosrc.use_cursor(main_c);
    EXPECT_EQ(gosrc.get_pos(), 40);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 163);

    gosrc.use_cursor(world_c);
    EXPECT_EQ(gosrc.get_pos(), 100);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 100);

    gosrc.use_cursor(wor_c);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(orl_c);
    gosrc.start_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(rld_c);
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 100);

    gosrc.use_cursor(rld_eol_c);
    EXPECT_EQ(gosrc.get_pos(), 91);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 103);

    gosrc.use_cursor(print_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(print_wor_c);
    EXPECT_EQ(gosrc.get_pos(), 85);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 91);

    gosrc.use_cursor(func_world_c);
    EXPECT_EQ(gosrc.get_pos(), 165);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 175);

    gosrc.use_cursor(if_c);
    EXPECT_EQ(gosrc.get_pos(), 52);
    gosrc.end_of_buffer();
    EXPECT_EQ(gosrc.get_pos(), 77);
}
