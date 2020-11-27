CXX ?= g++

INCLUDES += -Ivendor/ChaiScript/include
INCLUDES += -Ivendor/immer
INCLUDES += -Ivendor/spdlog/include
INCLUDES += -Iinclude

SPDLOG_LIB = build/spdlog/libspdlog.a
LIBS = $(SPDLOG_LIB)

CPPFLAGS = -std=c++17 ${INCLUDES} -O3 -DCHAISCRIPT_NO_THREADS
LDFLAGS = -Wl,--no-as-needed -ldl

TARGET_BIN = build/bin/mime
TARGET_LIB = build/lib/libmime.a

LIB_SRCS = $(shell ls src/lib/*.cc)
LIB_OBJS = $(addprefix build/.objs/,$(subst .cc,.o,$(LIB_SRCS)))
BIN_SRCS = $(shell ls src/cmd/*.cc)
BIN_OBJS = $(addprefix build/.objs/,$(subst .cc,.o,$(BIN_SRCS)))

DEP = $(LIB_OBJS:%.o=%.d) $(BIN_OBJS:%.o=%.d)
-include $(DEP)

ALL_SRCS=$(shell find src unittests -type f -name '*.cc')
ALL_HEADERS=$(shell find src include unittests -type f -name '*.hh')

build: bin lib

init:
	git submodule update --init --recursive vendor/*

lib: $(TARGET_LIB)
bin:  ${TARGET_BIN}

clean:
	rm -rf build

format:
	clang-format -i $(ALL_SRCS) $(ALL_HEADERS)

tidy: format
	clang-tidy --checks=readability-*,performance-*,cppcoreguidelines-*,bugprone-*,misc-* $(ABS_HEADERS) $(ABS_SRCS) -- $(CPPFLAGS)
	make format

${TARGET_LIB}: ${LIB_OBJS}
	mkdir -p build/lib
	ar r $@ $^

${TARGET_BIN}: ${BIN_OBJS} $(LIBS) $(TARGET_LIB)
	mkdir -p build/bin
	$(CXX) ${LDFLAGS} -o $@ $^

build/.objs/%.o: %.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -MMD -c -o $@ $<

INSTALL_PATH = $(shell systemd-path user-binaries)
install: build
	@test "${INSTALL_PATH}" == "" && echo -e "\nunable to get 'user-binaries' path from 'systemd-path' command\n" || install $(TARGET_BIN) ${INSTALL_PATH}/

## spdlog

build/spdlog:
	mkdir -p $@

$(SPDLOG_LIB): build/spdlog
	cd build/spdlog && cmake ../../vendor/spdlog && make -j

## unit test targets
TEST_BIN = build/bin/test
GTEST_LIB = build/googletest/lib/libgtest.a
TEST_SRCS = $(shell ls src/*/*.cc unittests/*.cc | grep -v main.cc)
TEST_OBJS = $(addprefix build/test/.objs/,$(subst .cc,.o,$(TEST_SRCS)))

TEST_CPPFLAGS = -std=c++17 ${INCLUDES} -Ivendor/googletest/googletest/include -O0 -g --coverage -DCHAISCRIPT_NO_THREADS
TEST_LDFLAGS = $(LDFLAGS) -pthread --coverage

testCover: test
	gcovr -r . -f src --html --html-details -o build/coverage.html

test: bin $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN):  $(TEST_OBJS) $(LIBS) $(GTEST_LIB)
	mkdir -p build/bin
	$(CXX) $(TEST_LDFLAGS) -o $@ $^

build/test/.objs/%.o: %.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(TEST_CPPFLAGS) -MMD -c -o $@ $<

build/googletest:
	mkdir -p $@

$(GTEST_LIB): build/googletest
	cd build/googletest && cmake ../../vendor/googletest && make -j
