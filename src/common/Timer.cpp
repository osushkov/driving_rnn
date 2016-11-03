
#include "Timer.hpp"
#include <unistd.h>
#include <cmath>

static const unsigned MICRO_SECONDS_IN_SECOND = 1000000;

void Timer::Start(void) { gettimeofday(&startTime, NULL); }

void Timer::Stop(void) { gettimeofday(&endTime, NULL); }

void Timer::Sleep(float seconds) const {
  int wholeSeconds = static_cast<int>(floorf(seconds));
  unsigned microseconds = (seconds - wholeSeconds) * MICRO_SECONDS_IN_SECOND;

  if (wholeSeconds > 0) {
    sleep(wholeSeconds);
  }

  usleep(microseconds);
}

float Timer::GetIntervalElapsedSeconds(void) const {
  unsigned seconds = endTime.tv_sec - startTime.tv_sec;
  unsigned mseconds = seconds * MICRO_SECONDS_IN_SECOND - startTime.tv_usec + endTime.tv_usec;

  return (float)mseconds / (float)MICRO_SECONDS_IN_SECOND;
}

unsigned Timer::GetIntervalElapsedMicroseconds(void) const {
  unsigned seconds = endTime.tv_sec - startTime.tv_sec;
  return seconds * MICRO_SECONDS_IN_SECOND - startTime.tv_usec + endTime.tv_usec;
}

float Timer::GetElapsedSeconds(void) const {
  struct timeval curTime;
  gettimeofday(&curTime, NULL);

  unsigned seconds = curTime.tv_sec - startTime.tv_sec;
  unsigned mseconds = seconds * MICRO_SECONDS_IN_SECOND - startTime.tv_usec + curTime.tv_usec;

  return (float)mseconds / (float)MICRO_SECONDS_IN_SECOND;
}

unsigned Timer::GetElapsedMicroseconds(void) const {
  struct timeval curTime;
  gettimeofday(&curTime, NULL);

  unsigned seconds = curTime.tv_sec - startTime.tv_sec;
  return seconds * MICRO_SECONDS_IN_SECOND - startTime.tv_usec + curTime.tv_usec;
}
