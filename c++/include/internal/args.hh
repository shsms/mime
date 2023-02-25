#ifndef MIME_ARGS_HH
#define MIME_ARGS_HH

#include <chaiscript/chaiscript.hpp>
#include <cxxopts.hpp>
#include <string>
#include <vector>

namespace mime {

class args_parser {
  public:
    args_parser(int argc, char **argv);
    void bool_opt(const std::string &key, const std::string &desc, const std::string &default_v);
    void bool_opt(const std::string &key, const std::string &desc);

    void int_opt(const std::string &key, const std::string &desc, const std::string &default_v);
    void int_opt(const std::string &key, const std::string &desc);

    void float_opt(const std::string &key, const std::string &desc, const std::string &default_v);
    void float_opt(const std::string &key, const std::string &desc);

    void string_opt(const std::string &key, const std::string &desc, const std::string &default_v);
    void string_opt(const std::string &key, const std::string &desc);

    std::string help();
    chaiscript::dispatch::Dynamic_Object parse();

  private:
    cxxopts::Options options;
    std::vector<std::string> bool_attr{};
    std::vector<std::string> int_attr{};
    std::vector<std::string> float_attr{};
    std::vector<std::string> string_attr{};
    int argc;
    char **argv;
};

} // namespace mime
#endif /* MIME_ARGS_HH */
