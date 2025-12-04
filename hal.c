#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <time.h>

#define HAL_SLEEP_MS 4

const struct timespec HAL_sleep_time = {
    .tv_sec = 0,
    .tv_nsec = HAL_SLEEP_MS * 1000000
};

uint32_t HAL_GetTick(void) {
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);

  return (uint64_t)now.tv_sec * 1000 + now.tv_nsec / 1000000;
}

void HAL_Sleep(void) {
  nanosleep(&HAL_sleep_time, NULL);
}