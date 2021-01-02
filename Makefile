install_path = $(shell systemd-path user-binaries)
target_bin = build/release/mime

.PHONY: default init clean build debug test testCover install

default: build

init:
	git submodule update --init --recursive vendor/*

clean:
	rm -rf build

build: CMakeLists.txt
	mkdir -p build/release && cd build/release && cmake -DCMAKE_BUILD_TYPE=Release ../.. && $(MAKE) -s

debug: CMakeLists.txt
	mkdir -p build/debug && cd build/debug && cmake -DCMAKE_BUILD_TYPE=Debug ../.. && $(MAKE) -s

test: build
	build/release/unittests

testCover: test
	gcovr -r . -f src --html --html-details -o build/coverage.html

install: build
	test "$(install_path)" == "" \
		&& echo -e "\nunable to get 'user-binaries' path from 'systemd-path' command\n" \
		|| install $(target_bin) $(install_path)/
