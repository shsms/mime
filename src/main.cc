#include "buffer.hh"
#include "chai_init.hh"
#include <fstream>

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wdefaulted-function-deleted"
#include <chaiscript/chaiscript.hpp>
// #pragma GCC diagnostic pop

int main(int argc, char* argv[]) {
    chaiscript::ChaiScript chai;

    chai.add(chaiscript::user_type<meme::buffer>(), "meme_buffer");
    chai.add(chaiscript::user_type<meme::text>(), "meme_text");
    chai.add(chaiscript::fun(&meme::buffer::contents), "meme_buffer_contents");

    chai.add(chaiscript::fun(&meme::buffer_text::get_buffer), "meme_buffer_text_get_buffer");
    chai.add(chaiscript::fun(&meme::buffer_text::get_text), "meme_buffer_text_get_text");
    chai.add(chaiscript::fun(&meme::buffer_bool::get_buffer), "meme_buffer_bool_get_buffer");
    chai.add(chaiscript::fun(&meme::buffer_bool::get_bool), "meme_buffer_bool_get_bool");

    chai.add(chaiscript::fun(&meme::get_string), "get_string");

    chai.add(chaiscript::fun([](meme::buffer &lhs, const meme::buffer &rhs) -> meme::buffer & {
                 return lhs = rhs;
             }),
             "=");
    chai.add(chaiscript::fun([](meme::text &lhs, const meme::text &rhs) -> meme::text & {
                 return lhs = rhs;
             }),
             "=");

    chai.add(chaiscript::fun(&meme::open), "open_impl");
    chai.add(chaiscript::fun(&meme::save), "save_impl");
    chai.add(chaiscript::fun(&meme::save_as), "save_as_impl");
    chai.add(chaiscript::fun(&meme::set_mark), "set_mark_impl");
    chai.add(chaiscript::fun(&meme::find), "find_impl");
    chai.add(chaiscript::fun(&meme::find_fuzzy), "find_fuzzy_impl");
    chai.add(chaiscript::fun(&meme::rfind), "rfind_impl");
    chai.add(chaiscript::fun(&meme::find_replace), "find_replace_impl");

    chai.add(chaiscript::fun(&meme::copy), "copy_impl");
    chai.add(chaiscript::fun(&meme::cut), "cut_impl");
    chai.add(chaiscript::fun(&meme::paste), "paste_impl");

    chai.add(chaiscript::fun(&meme::insert), "insert_impl");

    chai.add(chaiscript::fun(&meme::backward), "backward_impl");

    chai.eval(chai_init());

    chai.eval_file(argv[1]);
}
