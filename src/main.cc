#include "buffer.hh"
#include "chai_init.hh"
#include <fstream>

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wdefaulted-function-deleted"
#include <chaiscript/chaiscript.hpp>
// #pragma GCC diagnostic pop

void add_methods(chaiscript::ChaiScript &chai) {
    chai.add(chaiscript::user_type<mime::buffer>(), "mime_buffer");
    chai.add(chaiscript::user_type<mime::text>(), "mime_text");
    chai.add(chaiscript::fun(&mime::buffer::contents), "mime_buffer_contents");

    chai.add(chaiscript::fun(&mime::buffer_text::get_buffer), "mime_buffer_text_get_buffer");
    chai.add(chaiscript::fun(&mime::buffer_text::get_text), "mime_buffer_text_get_text");
    chai.add(chaiscript::fun(&mime::buffer_bool::get_buffer), "mime_buffer_bool_get_buffer");
    chai.add(chaiscript::fun(&mime::buffer_bool::get_bool), "mime_buffer_bool_get_bool");
    chai.add(chaiscript::fun(&mime::buffer_int::get_buffer), "mime_buffer_int_get_buffer");
    chai.add(chaiscript::fun(&mime::buffer_int::get_int), "mime_buffer_int_get_int");

    chai.add(chaiscript::fun(&mime::to_string), "to_string");

    chai.add(chaiscript::fun(&mime::open), "open_impl");
    chai.add(chaiscript::fun(&mime::save), "save_impl");
    chai.add(chaiscript::fun(&mime::save_as), "save_as_impl");
    chai.add(chaiscript::fun(&mime::set_mark), "set_mark_impl");
    chai.add(
        chaiscript::fun(
            static_cast<mime::buffer_bool (*)(mime::buffer, std::size_t, std::string, std::size_t)>(
                &mime::find)),
        "find_impl");
    chai.add(
        chaiscript::fun(
            static_cast<mime::buffer_bool (*)(mime::buffer, std::size_t, mime::text, std::size_t)>(
                &mime::find)),
        "find_impl");
    chai.add(
        chaiscript::fun(
            static_cast<mime::buffer_bool (*)(mime::buffer, std::size_t, std::string, std::size_t)>(
                &mime::rfind)),
        "rfind_impl");
    chai.add(
        chaiscript::fun(
            static_cast<mime::buffer_bool (*)(mime::buffer, std::size_t, mime::text, std::size_t)>(
                &mime::rfind)),
        "rfind_impl");
    chai.add(
        chaiscript::fun(
            static_cast<mime::buffer_bool (*)(mime::buffer, std::size_t, std::string, std::size_t)>(
                &mime::find_fuzzy)),
        "find_fuzzy_impl");
    chai.add(
        chaiscript::fun(
            static_cast<mime::buffer_bool (*)(mime::buffer, std::size_t, mime::text, std::size_t)>(
                &mime::find_fuzzy)),
        "find_fuzzy_impl");
    chai.add(chaiscript::fun(&mime::replace), "replace_impl");

    chai.add(chaiscript::fun(&mime::copy), "copy_impl");
    chai.add(chaiscript::fun(&mime::cut), "cut_impl");
    chai.add(chaiscript::fun(static_cast<mime::buffer (*)(mime::buffer, std::size_t, mime::text)>(
                 &mime::paste)),
             "paste_impl");
    chai.add(chaiscript::fun(static_cast<mime::buffer (*)(mime::buffer, std::size_t, std::string)>(
                 &mime::paste)),
             "paste_impl");
    chai.add(chaiscript::fun(&mime::erase_region), "erase_region_impl");
    chai.add(chaiscript::fun(&mime::clear), "clear_impl");

    chai.add(chaiscript::fun(&mime::new_cursor), "new_cursor_impl");
    chai.add(chaiscript::fun(&mime::get_pos), "get_pos_impl");
    chai.add(chaiscript::fun(&mime::goto_pos), "goto_pos_impl");

    chai.add(chaiscript::fun(&mime::forward), "forward_impl");
    chai.add(chaiscript::fun(&mime::backward), "backward_impl");
    chai.add(chaiscript::fun(&mime::next_line), "next_line_impl");
    chai.add(chaiscript::fun(&mime::prev_line), "prev_line_impl");
    chai.add(chaiscript::fun(&mime::start_of_buffer), "start_of_buffer_impl");
    chai.add(chaiscript::fun(&mime::end_of_buffer), "end_of_buffer_impl");
    chai.add(chaiscript::fun(&mime::start_of_line), "start_of_line_impl");
    chai.add(chaiscript::fun(&mime::end_of_line), "end_of_line_impl");

    chai.add(chaiscript::fun([](mime::buffer &lhs, const mime::buffer &rhs) -> mime::buffer & {
                 return lhs = rhs;
             }),
             "=");
    chai.add(chaiscript::fun(
                 [](mime::text &lhs, const mime::text &rhs) -> mime::text & { return lhs = rhs; }),
             "=");
    chai.add(chaiscript::fun([](const std::string &lhs, const mime::text &rhs) -> mime::text {
                 std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
                 std::u32string u32lhs = cvt.from_bytes(lhs);
                 return mime::text{u32lhs.begin(), u32lhs.end()} + rhs;
             }),
             "+");
    chai.add(chaiscript::fun([](const mime::text &lhs, const std::string &rhs) -> mime::text {
                 std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
                 std::u32string u32rhs = cvt.from_bytes(rhs);
                 return lhs + mime::text{u32rhs.begin(), u32rhs.end()};
             }),
             "+");
    chai.add(chaiscript::fun(
                 [](const mime::text &lhs, const mime::text &rhs) -> bool { return lhs == rhs; }),
             "==");
    chai.add(chaiscript::fun([](const std::string &lhs, const mime::text &rhs) -> bool {
                 if (lhs.size() > rhs.size()) {
                     return lhs == mime::to_string(rhs);
                 }
                 return mime::text{lhs.begin(), lhs.end()} == rhs;
             }),
             "==");
    chai.add(chaiscript::fun([](const mime::text &lhs, const std::string &rhs) -> bool {
                 if (rhs.size() > lhs.size()) {
                     return rhs == mime::to_string(lhs);
                 }
                 return mime::text{rhs.begin(), rhs.end()} == lhs;
             }),
             "==");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n"
                  << "\twhere <filename> is the name of the file containing the mime script to "
                     "execute.\n\n";
        return 1;
    }

    chaiscript::ChaiScript chai;
    add_methods(chai);
    chai.eval(chai_init());
    chai.eval_file(argv[1]);
}
