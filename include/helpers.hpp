#ifndef include_helpers_hpp
#define include_helpers_hpp

#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>

namespace chrono = std::chrono;

class Timer {
private:
  chrono::time_point<chrono::high_resolution_clock> start;
  chrono::time_point<chrono::high_resolution_clock> end;

public:
  void start_timer() { this->start = chrono::high_resolution_clock::now(); }
  void stop_timer() { this->end = chrono::high_resolution_clock::now(); }

  auto time_elapsed() {
    return chrono::duration_cast<chrono::microseconds>(this->end - this->start)
        .count();
  }
};

void assert_int(size_t expected, size_t actual, std::string str) {
  if (expected != actual) {
    std::cerr << str << " expected: " << expected << " actual: " << actual
              << std::endl;
    assert(expected == actual);
  }
}

void assert_float(float expected, float actual, std::string str) {
  float diff = fabs(expected - actual);
  float loss = (diff / expected) * 100;
  if (loss > 0.1) {
    std::cerr << str << " expected: " << expected << " actual: " << actual
              << " loss: " << loss << std::endl;
    assert(loss < 0.1);
  }
}

#endif /* include_helpers_hpp */
