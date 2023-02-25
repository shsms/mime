cmake_minimum_required(VERSION 3.15)

project(mime)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(
  include
  vendor/immer
  vendor/cxxopts/include
  vendor/ChaiScript/include
  vendor/spdlog/include)

add_library(mime STATIC
  src/lib/mime.cc
  src/lib/u32utils.cc)

add_executable(mime-bin
  src/cmd/chai.cc
  src/cmd/main.cc)

target_compile_definitions(mime-bin PUBLIC CHAISCRIPT_NO_THREADS)
target_link_libraries(mime-bin mime dl)
set_target_properties(mime-bin
  PROPERTIES OUTPUT_NAME mime)

## build test

add_subdirectory(vendor/googletest)
add_executable(unittests
  src/lib/mime.cc
  src/lib/u32utils.cc
  src/cmd/chai.cc
  unittests/args_test.cc
  unittests/buffer_test.cc
  unittests/except_test.cc
  unittests/narrow_test.cc
  unittests/chai_test.cc
  unittests/main_test.cc
  unittests/u32utils_test.cc)
target_compile_options(unittests PUBLIC -coverage)
target_link_libraries(unittests gtest dl)
target_link_options(unittests PUBLIC -coverage)
