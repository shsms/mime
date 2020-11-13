CXX=g++

INCLUDES += -Ivendor/ChaiScript/include
INCLUDES += -Ivendor/immer/

CPPFLAGS = -std=c++17 ${INCLUDES} -ggdb3
LDFLAGS = -ldl -lpthread

TARGET_BIN = bin/tpie

SRCS = $(shell cd src && find * -type f -not -name '*_test.cc' -name '*.cc')
OBJS = $(addprefix build/.objs/,$(subst .cc,.o,$(SRCS)))

build: bin ${TARGET_BIN}

bin:
	mkdir -p bin

clean:
	rm -rf build bin

${TARGET_BIN}: ${OBJS}
	$(CXX) ${LDFLAGS} -o $@ $^

build/.objs/%.o: src/%.cc
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CPPFLAGS) -MMD -c -o $@ $<

run: build
	${TARGET_BIN}
