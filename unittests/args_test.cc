#include <gtest/gtest.h>
#include <immer/box.hpp>
#include <mime/args.hh>
#include <string>

using namespace std::string_literals;

// Argv class from the cxxopts project:
// https://github.com/jarro2783/cxxopts/blob/2_1/test/options.cpp#L7
class Argv {
  public:
    Argv(){}
    Argv& operator=(Argv&&) = default;
    
    Argv(std::initializer_list<const char *> args)
        : m_argv(new char *[args.size()]), m_argc(args.size()) {
        int i = 0;
        auto iter = args.begin();
        while (iter != args.end()) {
            auto len = strlen(*iter) + 1;
            auto ptr = std::unique_ptr<char[]>(new char[len]);

            strcpy(ptr.get(), *iter);
            m_args.push_back(std::move(ptr));
            m_argv.get()[i] = m_args.back().get();

            ++iter;
            ++i;
        }
    }

    char **argv() const { return m_argv.get(); }

    int argc() const { return m_argc; }

  private:
    std::vector<std::unique_ptr<char[]>> m_args;
    std::unique_ptr<char *[]> m_argv;
    int m_argc;
};

class ArgsTest : public ::testing::Test {
  protected:
    void SetUp() override {
        args = {
	    "mime",
	    "test.mime",
	    "pos0",
	    "--bool_v",
	    "--float_v",
	    "2.4",
	    "-i",
	    "5",
	    "--string_v",
	    "hello",
	    "pos1"
	};
    };

    Argv args;
};

TEST_F(ArgsTest, AllTypes) {
    args_parser parser(args.argc(), args.argv());
    parser.bool_opt("bool_v", "bool value");
    parser.int_opt("i,int_v", "int value");
    parser.float_opt("float_v", "float value");
    parser.string_opt("string_v", "string value");
    
    auto r = parser.parse();

    EXPECT_EQ(chaiscript::boxed_cast<bool>(r.get_attr("bool_v")), true);
    EXPECT_EQ(chaiscript::boxed_cast<float>(r.get_attr("float_v")), 2.4f);
    EXPECT_EQ(chaiscript::boxed_cast<int>(r.get_attr("int_v")), 5);
    EXPECT_EQ(chaiscript::boxed_cast<std::string>(r.get_attr("string_v")), "hello"s);
    auto positional = chaiscript::boxed_cast<std::vector<chaiscript::Boxed_Value>>(r.get_attr("positional"));
    EXPECT_EQ(positional.size(), 2);
    EXPECT_EQ(chaiscript::boxed_cast<std::string>(positional[0]), "pos0"s);
    EXPECT_EQ(chaiscript::boxed_cast<std::string>(positional[1]), "pos1"s);

    EXPECT_NE(parser.help().find("Usage"), std::string::npos);
}
