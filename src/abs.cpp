#include "helpers.hpp"
#include <complex>
#include <iostream>
#include <memory>
#include <ostream>
#include <tmmintrin.h>
#include <valarray>
#include <vector>
#define amd64

#ifdef amd64
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <xmmintrin.h>
#endif

#ifdef arm64
#include "sse2neon.h"
#endif

Timer stop_watch;

void abs_sse(std::vector<int> &arr, std::vector<int> &abs_arr) {
  assert(arr.size() < (size_t)INT_MAX);

  const int simd_width = 4;
  size_t quot = arr.size() / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {
    const __m128i temp_arr =
        _mm_set_epi32(arr[i + 3], arr[i + 2], arr[i + 1], arr[i]);
    __m128i arr_r = _mm_abs_epi32(temp_arr);

    for (size_t j = 0; j < simd_width; j++) {
      abs_arr[i + j] = _mm_cvtsi128_si32(arr_r);
      arr_r = _mm_srli_si128(arr_r, 4);
    }
  }

  for (size_t i = limit; i < arr.size(); i++) {
    abs_arr[i] = abs(arr[i]);
  }
}

void abs_avx(std::vector<int> &arr, std::vector<int> &abs_arr) {
  assert(arr.size() < (size_t)INT_MAX);

  const int simd_width = 8;
  size_t quot = arr.size() / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {
    const __m256i temp_arr =
        _mm256_set_epi32(arr[i], arr[i + 1], arr[i + 2], arr[i + 3], arr[i + 4],
                         arr[i + 5], arr[i + 6], arr[i + 7]);
    __m256i arr_r = _mm256_abs_epi32(temp_arr);

    for (size_t j = 0; j < 8; j++) {
      abs_arr[i + (7 - j)] = _mm256_cvtsi256_si32(arr_r - j);
    }
  }

  for (size_t i = limit; i < arr.size(); i++) {
    abs_arr[i] = abs(arr[i]);
  }
}

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << " usage: build/abs <N>" << std::endl;
    return 1;
  }
  size_t N = atoi(argv[1]);

  std::vector<int> arr(N);
  std::valarray<int> valarr(N);
  time_t seed = time(0);
  std::cout << "Seed: " << seed << std::endl;
  srand(seed);

  std::cout << "Array dim : " << N << std::endl;

  int offset = -500;
  float range = 1000.0f;
  for (size_t i = 0; i < N; i++) {
    // arr[i] = offset + range * (rand() / (float)RAND_MAX);
    arr[i] = -i;
  }

  std::vector<int> expected(N);

#ifdef GOLDEN
  stop_watch.start_timer();
  for (size_t i = 0; i < N; i++) {
    expected[i] = abs(arr[i]);
  }
  stop_watch.stop_timer();
  std::cout << "Elapsed time GOLDEN " << stop_watch.time_elapsed() << " us"
            << std::endl;
#endif

  std::vector<int> sse_actual(N);
#ifdef SSE
  stop_watch.start_timer();
  abs_sse(arr, sse_actual);
  stop_watch.stop_timer();
  std::cout << "Elapsed time SSE " << stop_watch.time_elapsed() << " us"
            << std::endl;
#endif

#ifdef ASSERT
  for (size_t i = 0; i < N; i++) {
    assert_int(expected[i], sse_actual[i], "SSE");
  }
  std::cout << "Assertion is successful for SSE" << std::endl;
#endif

  std::vector<int> avx_actual(N);
#ifdef AVX
  stop_watch.start_timer();
  abs_avx(arr, avx_actual);
  stop_watch.stop_timer();
  std::cout << "Elapsed time AVX " << stop_watch.time_elapsed() << " us"
            << std::endl;
#endif

#ifdef ASSERT
  for (size_t i = 0; i < N; i++) {
    if (expected[i] != avx_actual[i]) {
      std::cout << "expected : " << expected[i] << " ";
      std::cout << "actual : " << avx_actual[i] << " ";
      std::cout << "at position : " << i << std::endl;
    }
    assert_int(expected[i], avx_actual[i], "AVX");
  }
  std::cout << "Assertion is successful for AVX" << std::endl;
#endif
}