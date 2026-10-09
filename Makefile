.PHONY: all configure build test clean release

BUILD_DIR := build

all: build test

configure:
	cmake -S . -B $(BUILD_DIR) \
		-DCMAKE_BUILD_TYPE=Debug \
		-DMIP_BUILD_TESTS=ON \
		-DMIP_BUILD_EXAMPLES=ON

build: configure
	cmake --build $(BUILD_DIR) --parallel

test: build
	ctest --test-dir $(BUILD_DIR) --output-on-failure

release:
	cmake -S . -B $(BUILD_DIR)-release \
		-DCMAKE_BUILD_TYPE=Release \
		-DMIP_BUILD_TESTS=ON \
		-DMIP_BUILD_EXAMPLES=ON
	cmake --build $(BUILD_DIR)-release --parallel
	ctest --test-dir $(BUILD_DIR)-release --output-on-failure

clean:
	cmake -E rm -rf $(BUILD_DIR) $(BUILD_DIR)-release