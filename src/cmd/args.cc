#include <mime/args.hh>

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
