all: build

build/CMakeCache.txt:
	cmake -S . -B build

build: build/CMakeCache.txt
	cmake --build build

clean:
	rm -rf build

.PHONY: all build clean