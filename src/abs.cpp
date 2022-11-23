#include "helpers.hpp"
#include <complex>
#include <iostream>
#include <ostream>
#include <valarray>
#include <vector>

Timer stop_watch;

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << " usage: build/abs <N>" << std::endl;
    return 1;
  }
  size_t N = atoi(argv[1]);

  std::vector<float> arr(N);
  std::valarray<float> valarr(N);
  time_t seed = time(0);
  std::cout << "Seed: " << seed << std::endl;
  srand(seed);

  std::cout << "Array dim : " << N << std::endl;

  float offset = -500.0f;
  float range = 1000.0f;
  for (size_t i = 0; i < N; i++) {
    float num = offset + range * (rand() / (float)RAND_MAX);
    arr[i] = num;
    valarr[i] = num;
  }

  std::vector<float> expected(N);

#ifdef GOLDEN
  stop_watch.start_timer();
  for (size_t i = 0; i < N; i++) {
    expected[i] = abs(arr[i]);
  }
  stop_watch.stop_timer();
  std::cout << "Elapsed time GOLDEN " << stop_watch.time_elapsed() << " us"
            << std::endl;
#endif
}