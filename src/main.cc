#include "buffer.hh"
#include "chai_init.hh"
#include <chaiscript/chaiscript.hpp>
#include <fstream>

int main() {
    chaiscript::ChaiScript chai;

    chai.add(chaiscript::user_type<meme::buffer>(), "meme_buffer");
    chai.add(chaiscript::fun([](meme::buffer &lhs, const meme::buffer &rhs) -> meme::buffer & {
                 return lhs = rhs;
             }),
             "=");

    auto lib = std::make_shared<chaiscript::Module>();
    chaiscript::bootstrap::standard_library::string_type<std::wstring>("wstring", *lib);
    chai.add(lib);

    chai.add(chaiscript::fun(&meme::open_file), "open_file_impl");
    chai.add(chaiscript::fun(&meme::find), "find_impl");
    chai.add(chaiscript::fun(&meme::set_mark), "set_mark_impl");
    chai.add(chaiscript::fun(&meme::copy), "copy_impl");

    chai.eval(chai_init());
    chai.eval_file("myfile.chai");
}
