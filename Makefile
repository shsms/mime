CXX=g++

INCLUDES += -Ivendor/ChaiScript/include
INCLUDES += -Ivendor/immer/

CPPFLAGS = -std=c++17 ${INCLUDES} -O3
LDFLAGS = -ldl -lpthread

TARGET_BIN = bin/mime

SRCS = $(shell cd src && find * -type f -not -name '*_test.cc' -name '*.cc')
OBJS = $(addprefix build/.objs/,$(subst .cc,.o,$(SRCS)))

ABS_SRCS = $(addprefix src/,$(SRCS))
ABS_HEADERS = $(shell find src -type f -name '*.hh')

DEP = $(OBJS:%.o=%.d)
-include $(DEP)

build: bin ${TARGET_BIN}

run: build
	${TARGET_BIN} ulysses.chai

bin:
	mkdir -p bin

clean:
	rm -rf build bin

format:
	clang-format -i $(ABS_SRCS) $(ABS_HEADERS)

tidy: format
	clang-tidy --checks=readability-*,performance-*,cppcoreguidelines-*,bugprone-*,misc-* $(ABS_HEADERS) $(ABS_SRCS) -- $(CPPFLAGS)
	make format

${TARGET_BIN}: ${OBJS}
	$(CXX) ${LDFLAGS} -o $@ $^

build/.objs/%.o: src/%.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -MMD -c -o $@ $<

build/googletest:
	mkdir -p build/googletest

build/googletest/lib/libgtest_main.a: build/googletest
	cd build/googletest && cmake ../../vendor/googletest && make

init:
	git submodule update --init --recursive vendor/*
