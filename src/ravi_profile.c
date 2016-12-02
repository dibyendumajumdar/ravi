#include "ravi_profile.h"
#include <stdint.h>

#ifdef _WIN32

#include <windows.h>


unsigned long long raviV_profiledata[NUM_OPCODES];

/* The number of nanoseconds in one second. */
#define UV__NANOSEC 1000000000

/* Interval (in seconds) of the high-resolution clock. */
static double hrtime_interval_ = 0;
static uint64_t prev_time = 0;

void raviV_init_profiledata(void) {
  LARGE_INTEGER perf_frequency;
  /* Retrieve high-resolution timer frequency
   * and precompute its reciprocal.
   */
  if (QueryPerformanceFrequency(&perf_frequency)) {
    hrtime_interval_ = 1.0 / perf_frequency.QuadPart;
  } else {
    hrtime_interval_= 0;
  }
}

void raviV_add_profiledata(OpCode opcode) {
  LARGE_INTEGER counter;

  /* If the performance interval is zero, there's no support. */
  if (hrtime_interval_ == 0) {
    return;
  }

  if (!QueryPerformanceCounter(&counter)) {
    return;
  }

  /* Because we have no guarantee about the order of magnitude of the
   * performance counter interval, integer math could cause this computation
   * to overflow. Therefore we resort to floating point math.
   */
  uint64_t this_time = (uint64_t) ((double) counter.QuadPart * hrtime_interval_ * UV__NANOSEC);
  raviV_profiledata[opcode] += (this_time - (prev_time == 0 ? this_time : prev_time));
  prev_time = this_time;
}
void raviV_print_profiledata(void) {
    for (int i = 0; i < NUM_OPCODES; i++) {
        printf("PerfStat [%s] %llu\n", luaP_opnames[i], raviV_profiledata[i]);
    }
} 


#endif
