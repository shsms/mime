#include <chaiscript/chaiscript.hpp>
#include <mime/args.hh>
#include <mime/except.hh>
#include <mime/mime.hh>
#include <mime/u32utils.hh>

namespace mime {

void add_bindings(chaiscript::ChaiScript &chai) {
    using long_str_t = long (buffer::*)(std::string);
    using long_txt_t = long (buffer::*)(text);
    using long_rex_t = long (buffer::*)(regex_t);
    using void_str_t = void (buffer::*)(std::string);
    using void_txt_t = void (buffer::*)(text);

    using replace_t = int (buffer::*)(std::string, std::string, std::size_t);
    using replace_all_t = int (buffer::*)(std::string, std::string);

    chai.add(chaiscript::user_type<text>(), "text");
    chai.add(chaiscript::fun(&text::empty), "empty");
    chai.add(chaiscript::fun(&text::size), "size");
    chai.add(chaiscript::constructor<text(const text &)>(), "text");

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
    chai.add(chaiscript::constructor<buffer(const std::string &, buffer::open_spec)>(), "buffer");
    chai.add(chaiscript::constructor<buffer(const std::string &)>(), "buffer");
    chai.add(chaiscript::constructor<buffer(const mime::text &)>(), "buffer");

    chai.add(chaiscript::fun(&buffer::empty), "empty");
    chai.add(chaiscript::fun(&buffer::narrowed), "narrowed");
    chai.add(chaiscript::fun(&buffer::get_name), "get_name");
    chai.add(chaiscript::fun(&buffer::get_contents), "get_contents");
    chai.add(chaiscript::fun(&buffer::size), "size");

    chai.add(chaiscript::fun(&buffer::save), "save");
    chai.add(chaiscript::fun(&buffer::save_as), "save_as");
    chai.add(chaiscript::fun(&buffer::set_mark), "set_mark");

    chai.add(chaiscript::fun(static_cast<long_str_t>(&buffer::find)), "find");
    chai.add(chaiscript::fun(static_cast<long_txt_t>(&buffer::find)), "find");
    chai.add(chaiscript::fun(static_cast<long_rex_t>(&buffer::find)), "find");

    chai.add(chaiscript::fun(static_cast<long_str_t>(&buffer::rfind)), "rfind");
    chai.add(chaiscript::fun(static_cast<long_txt_t>(&buffer::rfind)), "rfind");

    chai.add(chaiscript::fun(static_cast<long_str_t>(&buffer::find_fuzzy)), "find_fuzzy");
    chai.add(chaiscript::fun(static_cast<long_txt_t>(&buffer::find_fuzzy)), "find_fuzzy");

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

    // exit
    chai.add(chaiscript::fun([](int code) { throw exit_exception(code); }), "exit");
}

void add_args_bindings(chaiscript::ChaiScript &chai, args_parser &args) {
    chai.add(chaiscript::user_type<args_parser>(), "args_parser_t");
    chai.add(chaiscript::fun([&]() { return args; }), "args_parser");
    chai.add(chaiscript::fun(&args_parser::help), "help");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &,
                                                   const std::string &)>(&args_parser::bool_opt)),
             "bool_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &)>(
                     &args_parser::bool_opt)),
             "bool_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &,
                                                   const std::string &)>(&args_parser::int_opt)),
             "int_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &)>(
                     &args_parser::int_opt)),
             "int_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &,
                                                   const std::string &)>(&args_parser::float_opt)),
             "float_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &)>(
                     &args_parser::float_opt)),
             "float_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &,
                                                   const std::string &)>(&args_parser::string_opt)),
             "string_opt");
    chai.add(chaiscript::fun(
                 static_cast<void (args_parser::*)(const std::string &, const std::string &)>(
                     &args_parser::string_opt)),
             "string_opt");
    chai.add(chaiscript::fun(&args_parser::parse), "parse");
}

int run(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n"
                  << "\twhere <filename> is the name of the file containing the mime script to "
                     "execute.\n\n";
        return 1;
    }

    args_parser args(argc, argv);
    chaiscript::ChaiScript chai;
    add_bindings(chai);
    add_args_bindings(chai, args);

    try {
        chai.eval_file(argv[1]);
    } catch (const exit_exception &e) {
        return e.get_code();
    }

    return 0;
}


args_parser::args_parser(int argc, char **argv) : options(argv[1]), argc(argc - 1), argv(++argv) {}

void add_attr(const std::string &key, std::vector<std::string> &attr_vec) {
    auto commapos = key.find(',');
    if (commapos == std::string::npos) {
        attr_vec.push_back(key);
    } else {
        attr_vec.push_back(key.substr(commapos + 1));
    }
}

void args_parser::bool_opt(const std::string &key, const std::string &desc,
                           const std::string &default_v) {
    add_attr(key, bool_attr);
    options.add_options()(key, desc, cxxopts::value<bool>()->default_value(default_v));
}

void args_parser::bool_opt(const std::string &key, const std::string &desc) {
    bool_opt(key, desc, "false");
}

void args_parser::int_opt(const std::string &key, const std::string &desc,
                          const std::string &default_v) {
    add_attr(key, int_attr);
    options.add_options()(key, desc, cxxopts::value<int>()->default_value(default_v));
}

void args_parser::int_opt(const std::string &key, const std::string &desc) {
    int_opt(key, desc, "0");
}

void args_parser::float_opt(const std::string &key, const std::string &desc,
                            const std::string &default_v) {
    add_attr(key, float_attr);
    options.add_options()(key, desc, cxxopts::value<float>()->default_value(default_v));
}

void args_parser::float_opt(const std::string &key, const std::string &desc) {
    float_opt(key, desc, "0.0");
}

void args_parser::string_opt(const std::string &key, const std::string &desc,
                             const std::string &default_v) {
    add_attr(key, string_attr);
    options.add_options()(key, desc, cxxopts::value<std::string>()->default_value(default_v));
}

void args_parser::string_opt(const std::string &key, const std::string &desc) {
    string_opt(key, desc, "");
}

std::string args_parser::help() { return options.help(); }

chaiscript::dispatch::Dynamic_Object args_parser::parse() {
    chaiscript::dispatch::Dynamic_Object ret{};
    std::vector<std::string> positional;
    options.add_options()("positional", "positional arguments",
                          cxxopts::value<std::vector<std::string>>(positional));
    options.parse_positional("positional");
    auto results = options.parse(argc, argv);

    std::vector<chaiscript::Boxed_Value> boxed_positional{};
    std::transform(positional.begin(), positional.end(), std::back_inserter(boxed_positional),
                   [](auto &x) { return chaiscript::Boxed_Value(x); });

    ret.get_attr("positional") = chaiscript::Boxed_Value(boxed_positional);
    for (auto attr : bool_attr) {
        ret.get_attr(attr) = chaiscript::Boxed_Value(results[attr].as<bool>());
    }
    for (auto attr : int_attr) {
        ret.get_attr(attr) = chaiscript::Boxed_Value(results[attr].as<int>());
    }
    for (auto attr : float_attr) {
        ret.get_attr(attr) = chaiscript::Boxed_Value(results[attr].as<float>());
    }
    for (auto attr : string_attr) {
        ret.get_attr(attr) = chaiscript::Boxed_Value(results[attr].as<std::string>());
    }

    ret.set_explicit(true);
    return ret;
}

} // namespace mime
