#include <gtest/gtest.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

int main(int argc, char **argv) {
    spdlog::set_default_logger(spdlog::stderr_color_mt("stderr"));
    spdlog::set_pattern("[%^%l%$ %@/%!] %v");
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
