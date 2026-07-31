#include <stdint.h>
#include <stdio.h>
#include <time.h>

/* Compile with for example:
gcc -O3 -march=native xoroshiro128plus.c -o xoroshiro128plus */

/* Original xoroshiro128+ implementation from https://prng.di.unimi.it/xoroshiro128plus.c */

static inline uint64_t rotl(const uint64_t x, int k) {
  return (x << k) | (x >> (64 - k));
}

static uint64_t s[2];

uint64_t next(void) {
  const uint64_t s0 = s[0];
  uint64_t s1 = s[1];
  const uint64_t result = s0 + s1;

  s1 ^= s0;
  s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
  s[1] = rotl(s1, 37); // c

  return result;
}

/* This is the jump function for the generator. It is equivalent
   to 2^64 calls to next(); it can be used to generate 2^64
   non-overlapping subsequences for parallel computations. */
void jump(void) {
  static const uint64_t JUMP[] = { 0xdf900294d8f554a5, 0x170865df4b3201fc };

  uint64_t s0 = 0;
  uint64_t s1 = 0;
  for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
    for(int b = 0; b < 64; b++) {
      if (JUMP[i] & UINT64_C(1) << b) {
        s0 ^= s[0];
        s1 ^= s[1];
      }
      next();
    }

  s[0] = s0;
  s[1] = s1;
}

int main(void) {
  const int64_t n_samples = 1000LL * 1000 * 1000;
  int64_t n;
  uint64_t random_int64 = 0;
  struct timespec t_start, t_end;

  /* Initial state (matches the seed in ../correctness_test.f90) */
  s[0] = (uint64_t)1234;
  s[1] = (uint64_t)(-89234);

  printf(" Initial state (uint64_t): %20lu %20lu\n", s[0], s[1]);
  printf(" Initial state (int64_t):  %20lld %20lld\n",
         (long long)s[0], (long long)s[1]);

  clock_gettime(CLOCK_MONOTONIC, &t_start);
  for (n = 1; n <= n_samples; n++) {
    random_int64 = next();
  }
  clock_gettime(CLOCK_MONOTONIC, &t_end);

  double elapsed = (t_end.tv_sec - t_start.tv_sec)
    + (t_end.tv_nsec - t_start.tv_nsec) * 1e-9;

  printf(" Random value %ld is: %lu (uint64_t) \n", n_samples, random_int64);
  printf(" Random value %ld is: %lld (int64_t) \n",
         n_samples, (long long)random_int64);
  printf(" nanoseconds per number %g\n",
         1.0e9 * elapsed / (double)n_samples);

  jump();
  random_int64 = next();

  printf("\n Results after calling jump() \n");
  printf(" Random value is: %lu (uint64_t) \n", random_int64);
  printf(" Random value is: %lld (int64_t) \n", (long long)random_int64);

  return 0;
}
