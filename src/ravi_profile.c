#include "ravi_profile.h"

#include <lstate.h>

#include <stdint.h>

#if RAVI_BYTECODE_PROFILING_ENABLED
#ifdef _WIN32

#include <windows.h>

/* The number of nanoseconds in one second. */
#define UV__NANOSEC 1000000000

/* Interval (in seconds) of the high-resolution clock. */
static double hrtime_interval_ = 0;

void raviV_init_profiledata(lua_State *L) {
  LARGE_INTEGER perf_frequency;
  /* Retrieve high-resolution timer frequency
   * and precompute its reciprocal.
   */
  if (QueryPerformanceFrequency(&perf_frequency)) {
    hrtime_interval_ = 1.0 / perf_frequency.QuadPart;
  }
  else {
    hrtime_interval_ = 0;
  }
  global_State *g = G(L);
  g->ravi_profile1 = (unsigned long long *)(calloc(NUM_OPCODES, sizeof(unsigned long long)));
  g->ravi_profile2 = (unsigned long long *)(calloc(NUM_OPCODES, sizeof(unsigned long long)));
  g->ravi_prev_time = 0;
}

void raviV_add_profiledata(lua_State *L, OpCode opcode) {
  LARGE_INTEGER counter;

  /* If the performance interval is zero, there's no support. */
  if (hrtime_interval_ == 0) { return; }

  if (!QueryPerformanceCounter(&counter)) { return; }

  /* Because we have no guarantee about the order of magnitude of the
   * performance counter interval, integer math could cause this computation
   * to overflow. Therefore we resort to floating point math.
   */
  uint64_t this_time =
      (uint64_t)((double)counter.QuadPart * hrtime_interval_ * UV__NANOSEC);
  global_State *g = G(L);
  g->ravi_profile1[opcode] +=
      (this_time - (g->ravi_prev_time == 0 ? this_time : g->ravi_prev_time));
  g->ravi_prev_time = this_time;
  g->ravi_profile2[opcode]++;
}

void raviV_destroy_profiledata(lua_State *L) {
  global_State *g = G(L);
  for (int i = 0; i < NUM_OPCODES; i++) {
    printf("PerfStat [%s] %llu, %llu\n", luaP_opnames[i], g->ravi_profile1[i], g->ravi_profile2[i]);
  }
  free(g->ravi_profile1);
  free(g->ravi_profile2);
}

#else 

void raviV_init_profiledata(lua_State *L) {
  /* Not implemented */
}
void raviV_add_profiledata(lua_State *L, OpCode opcode) {
  /* Not implemented */
}
void raviV_destroy_profiledata(lua_State *L) {
  /* Not implemented */
}

#endif
#endif
