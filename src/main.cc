#include "buffer.hh"
#include "chai_init.hh"
#include <fstream>

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wdefaulted-function-deleted"
#include <chaiscript/chaiscript.hpp>
// #pragma GCC diagnostic pop

int main(int argc, char* argv[]) {
    chaiscript::ChaiScript chai;

    chai.add(chaiscript::user_type<mime::buffer>(), "mime_buffer");
    chai.add(chaiscript::user_type<mime::text>(), "mime_text");
    chai.add(chaiscript::fun(&mime::buffer::contents), "mime_buffer_contents");

    chai.add(chaiscript::fun(&mime::buffer_text::get_buffer), "mime_buffer_text_get_buffer");
    chai.add(chaiscript::fun(&mime::buffer_text::get_text), "mime_buffer_text_get_text");
    chai.add(chaiscript::fun(&mime::buffer_bool::get_buffer), "mime_buffer_bool_get_buffer");
    chai.add(chaiscript::fun(&mime::buffer_bool::get_bool), "mime_buffer_bool_get_bool");
    chai.add(chaiscript::fun(&mime::buffer_int::get_buffer), "mime_buffer_int_get_buffer");
    chai.add(chaiscript::fun(&mime::buffer_int::get_int), "mime_buffer_int_get_int");

    chai.add(chaiscript::fun(&mime::get_string), "get_string");

    chai.add(chaiscript::fun([](mime::buffer &lhs, const mime::buffer &rhs) -> mime::buffer & {
                 return lhs = rhs;
             }),
             "=");
    chai.add(chaiscript::fun([](mime::text &lhs, const mime::text &rhs) -> mime::text & {
                 return lhs = rhs;
             }),
             "=");

    chai.add(chaiscript::fun(&mime::open), "open_impl");
    chai.add(chaiscript::fun(&mime::save), "save_impl");
    chai.add(chaiscript::fun(&mime::save_as), "save_as_impl");
    chai.add(chaiscript::fun(&mime::set_mark), "set_mark_impl");
    chai.add(chaiscript::fun(&mime::find), "find_impl");
    chai.add(chaiscript::fun(&mime::find_fuzzy), "find_fuzzy_impl");
    chai.add(chaiscript::fun(&mime::rfind), "rfind_impl");
    chai.add(chaiscript::fun(&mime::replace), "replace_impl");

    chai.add(chaiscript::fun(&mime::copy), "copy_impl");
    chai.add(chaiscript::fun(&mime::cut), "cut_impl");
    chai.add(chaiscript::fun(&mime::paste), "paste_impl");

    chai.add(chaiscript::fun(&mime::insert), "insert_impl");
    chai.add(chaiscript::fun(&mime::new_cursor), "new_cursor_impl");
    chai.add(chaiscript::fun(&mime::get_cursor_pos), "get_cursor_pos_impl");

    chai.add(chaiscript::fun(&mime::backward), "backward_impl");

    chai.eval(chai_init());

    chai.eval_file(argv[1]);
}
