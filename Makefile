UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
	OMPFLAGS += -fopenmp
else ifeq ($(UNAME_S),Darwin)
	OMPFLAGS += -Xpreprocessor -fopenmp -lomp
endif

processor := $(shell uname -m)
ifeq ($(processor),$(filter $(processor),aarch64 arm64))
    ARCH_CFLAGS += -march=armv8-a+fp+simd+crc -D arm64 
	ifeq ($(UNAME_S),Darwin)
		EXTRA_FLAGS += -L /opt/homebrew/Cellar/libomp/15.0.4/lib
	endif
else ifeq ($(processor),$(filter $(processor),i386 x86_64))
    ARCH_CFLAGS += -march=native -D amd64 
endif

CXXFLAGS=-std=c++14 -Wall -Wextra -pedantic -I include -O3 -DGOLDEN
DEBUGFLAGS=-fsanitize=address -g

LIBS= -lcasa_casa -lcasa_meas -lcasa_measures

CXX=g++

all: dir build/abs

build/%: src/%.cpp
	$(CXX) -o $@ $< $(CXXFLAGS) $(OMPFLAGS) $(LIBS) $(ARCH_CFLAGS) $(EXTRA_FLAGS) 

clean:
	rm -rf build/* *app

dir:
	mkdir -p build

.PHONY: all clean

