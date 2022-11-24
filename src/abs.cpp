#include "helpers.hpp"
#include <climits>
#include <iostream>
#include <vector>

#ifdef amd64
#include <emmintrin.h>
#include <immintrin.h>
#include <smmintrin.h>
#include <tmmintrin.h>
#include <xmmintrin.h>
#endif

#ifdef arm64
#include <arm_neon.h>
#endif

Timer stop_watch;

#ifdef SSE
void abs_sse(std::vector<int> &arr, std::vector<int> &abs_arr) {
  assert(arr.size() < (size_t)INT_MAX);

  const int simd_width = 4;
  size_t quot = arr.size() / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {
    const __m128i temp_arr =
        _mm_load_si128(reinterpret_cast<const __m128i *>(arr.data() + i));

    __m128i arr_r = _mm_abs_epi32(temp_arr);

    _mm_store_si128(reinterpret_cast<__m128i *>(abs_arr.data() + i), arr_r);
  }

  for (size_t i = limit; i < arr.size(); i++) {
    abs_arr[i] = abs(arr[i]);
  }
}
#endif

#ifdef AVX
void abs_avx(std::vector<int> &arr, std::vector<int> &abs_arr) {
  assert(arr.size() < (size_t)INT_MAX);

  const int simd_width = 8;
  size_t quot = arr.size() / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {

    const __m256i temp_arr =
        _mm256_loadu_si256(reinterpret_cast<const __m256i *>(arr.data() + i));

    __m256i arr_r = _mm256_abs_epi32(temp_arr);

    _mm256_storeu_si256(reinterpret_cast<__m256i *>(abs_arr.data() + i), arr_r);
  }

  for (size_t i = limit; i < arr.size(); i++) {
    abs_arr[i] = abs(arr[i]);
  }
}
#endif

#ifdef NEON
void abs_neon(std::vector<int> &arr, std::vector<int> &abs_arr) {
  assert(arr.size() < (size_t)INT_MAX);

  const int simd_width = 4;
  size_t quot = arr.size() / simd_width;
  size_t limit = quot * simd_width;

  for (size_t i = 0; i < limit; i += simd_width) {
    int32x4_t temp_arr = vld1q_s32(arr.data() + i);

    int32x4_t arr_r = vabsq_s32(temp_arr);

    vst1q_s32(abs_arr.data() + i, arr_r);
  }

  for (size_t i = limit; i < arr.size(); i++) {
    abs_arr[i] = abs(arr[i]);
  }
}
#endif

int main(int argc, char **argv) {

  if (argc < 2) {
    std::cerr << " usage: build/abs <N>" << std::endl;
    return 1;
  }
  size_t N = atoi(argv[1]);

  std::vector<int> arr(N);
  time_t seed = time(0);
  std::cout << "Seed: " << seed << std::endl;
  srand(seed);

  std::cout << "Array dim : " << N << std::endl;

  int offset = -500;
  float range = 1000.0f;
  for (size_t i = 0; i < N; i++) {
    arr[i] = offset + range * (rand() / (float)RAND_MAX);
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

#ifdef SSE
  std::vector<int> sse_actual(N);
  stop_watch.start_timer();
  abs_sse(arr, sse_actual);
  stop_watch.stop_timer();
  std::cout << "Elapsed time SSE " << stop_watch.time_elapsed() << " us"
            << std::endl;

#ifdef ASSERT
  for (size_t i = 0; i < N; i++) {
    assert_int(expected[i], sse_actual[i], "SSE");
  }
  std::cout << "Assertion is successful for SSE" << std::endl;
#endif
#endif

#ifdef AVX
  std::vector<int> avx_actual(N);
  stop_watch.start_timer();
  abs_avx(arr, avx_actual);
  stop_watch.stop_timer();
  std::cout << "Elapsed time AVX " << stop_watch.time_elapsed() << " us"
            << std::endl;

#ifdef ASSERT
  for (size_t i = 0; i < N; i++) {
    assert_int(expected[i], avx_actual[i], "AVX");
  }
  std::cout << "Assertion is successful for AVX" << std::endl;
#endif
#endif

#ifdef NEON
  std::vector<int> neon_actual(N);
  stop_watch.start_timer();
  abs_neon(arr, neon_actual);
  stop_watch.stop_timer();
  std::cout << "Elapsed time NEON " << stop_watch.time_elapsed() << " us"
            << std::endl;

#ifdef ASSERT
  for (size_t i = 0; i < N; i++) {
    assert_int(expected[i], neon_actual[i], "NEON");
  }
  std::cout << "Assertion is successful for NEON" << std::endl;
#endif
#endif
}