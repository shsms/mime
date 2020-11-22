CXX=g++

INCLUDES += -Ivendor/ChaiScript/include
INCLUDES += -Ivendor/immer
INCLUDES += -Ivendor/spdlog/include

SPDLOG_LIB = build/spdlog/libspdlog.a
LIBS = $(SPDLOG_LIB)

CPPFLAGS = -std=c++17 ${INCLUDES} -O3
LDFLAGS = -ldl -lpthread

TARGET_BIN = bin/mime
SRCS = $(shell cd src && find * -type f -not -name '*_test.cc' -name '*.cc')
OBJS = $(addprefix build/.objs/,$(subst .cc,.o,$(SRCS)))
ABS_SRCS = $(addprefix src/,$(SRCS))
ABS_HEADERS = $(shell find src -type f -name '*.hh')

INSTALL_PATH = $(shell systemd-path user-binaries)

DEP = $(OBJS:%.o=%.d)
-include $(DEP)

build: bin ${TARGET_BIN}

run: build
	${TARGET_BIN} ulysses.chai

init:
	git submodule update --init --recursive vendor/*

bin:
	mkdir -p bin

clean:
	rm -rf build bin

format:
	clang-format -i $(ABS_SRCS) $(ABS_HEADERS)

tidy: format
	clang-tidy --checks=readability-*,performance-*,cppcoreguidelines-*,bugprone-*,misc-* $(ABS_HEADERS) $(ABS_SRCS) -- $(CPPFLAGS)
	make format

${TARGET_BIN}: ${OBJS} $(LIBS)
	$(CXX) ${LDFLAGS} -o $@ $^

build/.objs/%.o: src/%.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -MMD -c -o $@ $<

install: build
	@test "${INSTALL_PATH}" == "" && echo -e "\nunable to get 'user-binaries' path from 'systemd-path' command\n" || install $(TARGET_BIN) ${INSTALL_PATH}/

## spdlog

build/spdlog:
	mkdir -p $@

$(SPDLOG_LIB): build/spdlog
	cd build/spdlog && cmake ../../vendor/spdlog && make -j


## unit test targets
TEST_BIN = bin/test
GTEST_LIB = build/googletest/lib/libgtest.a
TEST_SRCS = $(shell cd src && find * -type f -not -name 'main.cc' -name '*.cc')
TEST_OBJS = $(addprefix build/test/.objs/,$(subst .cc,.o,$(TEST_SRCS)))

TEST_CPPFLAGS = -std=c++17 ${INCLUDES} -Ivendor/googletest/googletest/include -O0 -g --coverage
TEST_LDFLAGS = $(LDFLAGS) --coverage

testCover: test
	gcovr -r . -f src -e '.+_test\.cc$$' --html --html-details -o build/coverage.html

test: bin $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN):  $(TEST_OBJS) $(LIBS) $(GTEST_LIB)
	$(CXX) $(TEST_LDFLAGS) -o $@ $^

build/test/.objs/%.o: src/%.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(TEST_CPPFLAGS) -MMD -c -o $@ $<

build/googletest:
	mkdir -p $@

$(GTEST_LIB): build/googletest
	cd build/googletest && cmake ../../vendor/googletest && make -j
