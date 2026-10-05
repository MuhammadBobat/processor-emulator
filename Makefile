CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2
INCLUDES  = -Isrc

SRC  = src/emulator.cpp src/instructions.cpp
DEMO = build/demo

.PHONY: all run tidy sanitize clean

all: $(DEMO)

$(DEMO): $(SRC) demo/main.cpp src/*.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) demo/main.cpp -o $@

run: $(DEMO)
	cd build && ./demo

# Static analysis (requires clang-tidy)
tidy:
	clang-tidy $(SRC) demo/main.cpp -- -std=c++17 $(INCLUDES)

# Rebuild with AddressSanitizer and UBSan, then run
sanitize:
	@mkdir -p build
	$(CXX) -std=c++17 -g -fsanitize=address,undefined $(INCLUDES) $(SRC) demo/main.cpp -o build/demo_asan
	cd build && ./demo_asan

clean:
	rm -rf build
