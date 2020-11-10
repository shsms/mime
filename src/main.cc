#include "buffer.hh"
#include "chai_init.hh"
#include <fstream>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdefaulted-function-deleted"
#include <chaiscript/chaiscript.hpp>
#pragma GCC diagnostic pop

int main() {
    chaiscript::ChaiScript chai;

    // chai.add(chaiscript::user_type<meme::buffer>(), "meme_buffer");
    chai.add(chaiscript::fun(&meme::buffer::contents), "meme_buffer_contents");

    chai.add(chaiscript::fun(&meme::buffer_text::b), "meme_buffer_text_b");
    chai.add(chaiscript::fun(&meme::buffer_text::t), "meme_buffer_text_t");
    chai.add(chaiscript::fun(&meme::buffer_bool::b), "meme_buffer_bool_b");
    chai.add(chaiscript::fun(&meme::buffer_bool::success), "meme_buffer_bool_success");

    chai.add(chaiscript::fun(&meme::get_string), "get_string");

    chai.add(chaiscript::fun([](meme::buffer &lhs, const meme::buffer &rhs) -> meme::buffer & {
                 return lhs = rhs;
             }),
             "=");

    // auto lib = std::make_shared<chaiscript::Module>();
    // chaiscript::bootstrap::standard_library::string_type<std::wstring>("wstring", *lib);
    // chai.add(lib);

    chai.add(chaiscript::fun(&meme::open_file), "open_file_impl");
    chai.add(chaiscript::fun(&meme::search), "search_impl");
    chai.add(chaiscript::fun(&meme::set_mark), "set_mark_impl");
    chai.add(chaiscript::fun(&meme::copy), "copy_impl");
    chai.add(chaiscript::fun(&meme::cut), "cut_impl");

    chai.eval(chai_init());
    chai.eval_file("myfile.chai");
}
