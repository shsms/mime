install_path = $(shell systemd-path user-binaries)
target_bin = build/mime

default: build

init:
	git submodule update --init --recursive vendor/*

clean:
	rm -rf build

build: CMakeLists.txt
	mkdir -p build && cd build && cmake .. && make -j

test: build
	build/unittests

testCover: test
	gcovr -r . -f src --html --html-details -o build/coverage.html

install: build
	test "$(install_path)" == "" \
		&& echo -e "\nunable to get 'user-binaries' path from 'systemd-path' command\n" \
		|| install $(target_bin) $(install_path)/
