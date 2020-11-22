#include "buffer.hh"
#include "chai.hh"
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>\n"
                  << "\twhere <filename> is the name of the file containing the mime script to "
                     "execute.\n\n";
        return 1;
    }

    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
    spdlog::set_pattern("[%^%l%$ %@/%!] %v");

    mime::run(argv[1]);
}
