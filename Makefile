install_path = $(shell systemd-path user-binaries)
target_bin = build/release/mime

.PHONY: default init clean build debug test testCover package install format

default: build

init:
	git submodule update --init --recursive c++/vendor/*

clean:
	rm -rf build

build: c++/CMakeLists.txt
	mkdir -p build/release && cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../../c++ && $(MAKE) -s

debug: c++/CMakeLists.txt
	mkdir -p build/debug && cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../../c++ && $(MAKE) -s

test: build
	build/release/unittests

testCover: test
	gcovr -r . -f c++/src --html --html-details -o build/coverage.html

install: test
	test "$(install_path)" == "" \
		&& echo -e "\nunable to get 'user-binaries' path from 'systemd-path' command\n" \
		|| install $(target_bin) $(install_path)/

package:
	tar -czf mime-linux-amd64.tar.gz --directory=build/release mime
	tar -cJf mime-linux-amd64.tar.xz --directory=build/release mime

##

src = $(shell find c++/src c++/include c++/unittests -type f -name '*.cc' -o -name '*.hh')
format:
	clang-format -i $(src)

wasm:
	em++ \
		-O3 \
		--std=c++17 \
		-I vendor/ChaiScript/include \
		-I include \
		-I vendor/cxxopts/include \
		-I vendor/immer/ \
		-s DEMANGLE_SUPPORT=1 \
		-s MAIN_MODULE=1 \
		-s "EXPORTED_FUNCTIONS=['_run_script']" \
		-s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap']" \
		--pre-js docs/playground/emscripten-pre.js \
		-o docs/playground/mime.js \
		src/cmd/chai.cc src/lib/*.cc

playground-css:
	cd docs/playground && npx tailwindcss build -o tailwind.css
