#include <chaiscript/chaiscript.hpp>
#include <mime/mime.hh>
#include <mime/u32utils.hh>

namespace mime {

void add_bindings(chaiscript::ChaiScript &chai) {
    using bool_str_t = bool (buffer::*)(std::string);
    using bool_txt_t = bool (buffer::*)(text);
    using bool_rex_t = bool (buffer::*)(regex_t);
    using void_str_t = void (buffer::*)(std::string);
    using void_txt_t = void (buffer::*)(text);

    using replace_t = int (buffer::*)(std::string, std::string, std::size_t);
    using replace_all_t = int (buffer::*)(std::string, std::string);

    chai.add(chaiscript::user_type<text>(), "text");
    chai.add(chaiscript::user_type<regex_t>(), "regex_t");
    chai.add(chaiscript::fun(&to_string), "to_string");
    chai.add(chaiscript::fun(&regex), "regex");

    chai.add(chaiscript::fun(static_cast<void (*)(std::wstring &)>(&u32::ltrim)), "ltrim");
    chai.add(chaiscript::fun(static_cast<void (*)(text &)>(&u32::ltrim)), "ltrim");
    chai.add(chaiscript::fun(static_cast<void (*)(std::wstring &)>(&u32::rtrim)), "rtrim");
    chai.add(chaiscript::fun(static_cast<void (*)(text &)>(&u32::rtrim)), "rtrim");
    chai.add(chaiscript::fun(static_cast<void (*)(std::wstring &)>(&u32::trim)), "trim");
    chai.add(chaiscript::fun(static_cast<void (*)(text &)>(&u32::trim)), "trim");

    chai.add(chaiscript::user_type<buffer>(), "buffer");
    chai.add(chaiscript::constructor<buffer()>(), "buffer");
    chai.add(chaiscript::constructor<buffer(const std::string &)>(), "buffer");
    chai.add(chaiscript::constructor<buffer(const std::string &, buffer::open_spec)>(), "buffer");

    chai.add(chaiscript::fun(&buffer::empty), "empty");
    chai.add(chaiscript::fun(&buffer::get_contents), "get_contents");
    chai.add(chaiscript::fun(&buffer::size), "size");

    chai.add(chaiscript::fun(&buffer::save), "save");
    chai.add(chaiscript::fun(&buffer::save_as), "save_as");
    chai.add(chaiscript::fun(&buffer::set_mark), "set_mark");

    chai.add(chaiscript::fun(static_cast<bool_str_t>(&buffer::find)), "find");
    chai.add(chaiscript::fun(static_cast<bool_txt_t>(&buffer::find)), "find");
    chai.add(chaiscript::fun(static_cast<bool_rex_t>(&buffer::find)), "find");

    chai.add(chaiscript::fun(static_cast<bool_str_t>(&buffer::rfind)), "rfind");
    chai.add(chaiscript::fun(static_cast<bool_txt_t>(&buffer::rfind)), "rfind");

    chai.add(chaiscript::fun(static_cast<bool_str_t>(&buffer::find_fuzzy)), "find_fuzzy");
    chai.add(chaiscript::fun(static_cast<bool_txt_t>(&buffer::find_fuzzy)), "find_fuzzy");

    chai.add(chaiscript::fun(static_cast<replace_t>(&buffer::replace)), "replace");
    chai.add(chaiscript::fun(static_cast<replace_all_t>(&buffer::replace)), "replace");

    chai.add(chaiscript::fun(&buffer::copy), "copy");
    chai.add(chaiscript::fun(&buffer::cut), "cut");

    chai.add(chaiscript::fun(static_cast<void_str_t>(&buffer::paste)), "paste");
    chai.add(chaiscript::fun(static_cast<void_txt_t>(&buffer::paste)), "paste");

    chai.add(chaiscript::fun(&buffer::erase_region), "erase_region");
    chai.add(chaiscript::fun(&buffer::clear), "clear");
    chai.add(chaiscript::fun(&buffer::new_cursor), "new_cursor");
    chai.add(chaiscript::fun(&buffer::use_cursor), "use_cursor");
    chai.add(chaiscript::fun(&buffer::get_pos), "get_pos");
    chai.add(chaiscript::fun(&buffer::goto_pos), "goto_pos");

    chai.add(chaiscript::fun(static_cast<void (buffer::*)(std::size_t)>(&buffer::forward)),
             "forward");
    chai.add(chaiscript::fun(static_cast<void (buffer::*)()>(&buffer::forward)), "forward");
    chai.add(chaiscript::fun(static_cast<void (buffer::*)(std::size_t)>(&buffer::backward)),
             "backward");
    chai.add(chaiscript::fun(static_cast<void (buffer::*)()>(&buffer::backward)), "backward");
    chai.add(chaiscript::fun(static_cast<int (buffer::*)(std::size_t)>(&buffer::next_line)),
             "next_line");
    chai.add(chaiscript::fun(static_cast<int (buffer::*)()>(&buffer::next_line)), "next_line");
    chai.add(chaiscript::fun(static_cast<int (buffer::*)(std::size_t)>(&buffer::prev_line)),
             "prev_line");
    chai.add(chaiscript::fun(static_cast<int (buffer::*)()>(&buffer::prev_line)), "prev_line");

    chai.add(chaiscript::fun(&buffer::start_of_buffer), "start_of_buffer");
    chai.add(chaiscript::fun(&buffer::end_of_buffer), "end_of_buffer");
    chai.add(chaiscript::fun(&buffer::start_of_line), "start_of_line");
    chai.add(chaiscript::fun(&buffer::end_of_line), "end_of_line");
    chai.add(chaiscript::fun(&buffer::start_of_block), "start_of_block");
    chai.add(chaiscript::fun(&buffer::end_of_block), "end_of_block");
    chai.add(chaiscript::fun(&buffer::narrow_to_block), "narrow_to_block");
    chai.add(chaiscript::fun(&buffer::narrow_to_region), "narrow_to_region");
    chai.add(chaiscript::fun(&buffer::widen), "widen");

    chai.add(chaiscript::fun([](buffer &lhs, const buffer &rhs) -> buffer & { return lhs = rhs; }),
             "=");
    chai.add(chaiscript::fun([](text &lhs, const text &rhs) -> text & { return lhs = rhs; }), "=");

    chai.add(chaiscript::fun([](const std::string &lhs, const text &rhs) -> text {
                 std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
                 std::wstring u32lhs = cvt.from_bytes(lhs);
                 return text{u32lhs.begin(), u32lhs.end()} + rhs;
             }),
             "+");
    chai.add(chaiscript::fun([](const text &lhs, const std::string &rhs) -> text {
                 std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
                 std::wstring u32rhs = cvt.from_bytes(rhs);
                 return lhs + text{u32rhs.begin(), u32rhs.end()};
             }),
             "+");
    chai.add(chaiscript::fun([](const text &lhs, const text &rhs) -> text { return lhs + rhs; }),
             "+");

    chai.add(chaiscript::fun([](const text &lhs, const text &rhs) -> bool { return lhs == rhs; }),
             "==");
    chai.add(chaiscript::fun([](const std::string &lhs, const text &rhs) -> bool {
                 std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
                 std::wstring u32lhs = cvt.from_bytes(lhs);
                 if (u32lhs.size() != rhs.size()) {
                     return false;
                 }
                 return text{lhs.begin(), lhs.end()} == rhs;
             }),
             "==");
    chai.add(chaiscript::fun([](const text &lhs, const std::string &rhs) -> bool {
                 std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
                 std::wstring u32rhs = cvt.from_bytes(rhs);
                 if (u32rhs.size() != lhs.size()) {
                     return false;
                 }
                 return text{rhs.begin(), rhs.end()} == lhs;
             }),
             "==");

    // add enums
    chaiscript::ModulePtr m = chaiscript::ModulePtr(new chaiscript::Module());

    chaiscript::utility::add_class<buffer::open_spec>(
        *m, "open_spec", {{buffer::try_open, "try_open"}, {buffer::must_open, "must_open"}});

    chai.add(m);
}

void run(const std::string &filename) {
    chaiscript::ChaiScript chai;
    add_bindings(chai);
    chai.eval_file(filename);
}
} // namespace mime
