UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OMPFLAGS += -fopenmp
else ifeq ($(UNAME_S),Darwin)
	OMPFLAGS += -Xpreprocessor -fopenmp -lomp
endif

CXXFLAGS=-std=c++14 -Wall -Wextra -pedantic -I include -O3

processor := $(shell uname -m)
ifeq ($(processor),$(filter $(processor),aarch64 arm64))
    ARCH_C_FLAGS += -march=armv8-a+fp+simd+crc 
	CUSTOM_CODE_FLAGS += -DSSE2NEON -DASSERT 
else ifeq ($(processor),$(filter $(processor),i386 x86_64))
    ARCH_C_FLAGS += -march=native 
	CUSTOM_CODE_FLAGS += -DASSERT 
endif

DEBUGFLAGS=-fsanitize=address -g

CXX=g++

.PHONY: all clean dir

all: dir build/abs

build/%: src/%.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(OMPFLAGS) $(ARCH_C_FLAGS) $(CUSTOM_CODE_FLAGS) $(OPT)

clean:
	rm -rf build/* *app

scratch: scratch.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(ARCH_C_FLAGS) $(CUSTOM_CODE_FLAGS) $(OPT)

dir:
	mkdir -p build


