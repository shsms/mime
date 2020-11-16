CXX=g++

INCLUDES += -Ivendor/ChaiScript/include
INCLUDES += -Ivendor/immer/

CPPFLAGS = -std=c++17 ${INCLUDES} -O3
LDFLAGS = -ldl -lpthread

TARGET_BIN = bin/mime

SRCS = $(shell cd src && find * -type f -not -name '*_test.cc' -name '*.cc')
OBJS = $(addprefix build/.objs/,$(subst .cc,.o,$(SRCS)))

build: bin ${TARGET_BIN}

run: build
	${TARGET_BIN} ulysses.chai

bin:
	mkdir -p bin

clean:
	rm -rf build bin

${TARGET_BIN}: ${OBJS}
	$(CXX) ${LDFLAGS} -o $@ $^

build/.objs/%.o: src/%.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -MMD -c -o $@ $<


init:
	git submodule update --init --recursive vendor/*
