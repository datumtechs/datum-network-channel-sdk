#include "simple_timer.h"

#include <chrono>
using namespace std::chrono;

void SimpleTimer::start() {
  begin = steady_clock::now();
  end = begin;
  stoped = false;
}

double SimpleTimer::stop() {
  end = steady_clock::now();
  stoped = true;
  duration<double> elapsed_seconds = end - begin;
  double costTime = elapsed_seconds.count();
  return costTime;
}

double SimpleTimer::elapse() const {
  auto ending = steady_clock::now();
  if (stoped)
    ending = end;
  duration<double> elapsed_seconds = ending - begin;
  return elapsed_seconds.count();
}

long long int SimpleTimer::ms_elapse() const {
  auto ending = steady_clock::now();
  if (stoped)
    ending = end;
  duration<long long int, std::milli> elapsed_milliseconds =
    duration_cast<duration<long long int, std::milli>>(ending - begin);
  return elapsed_milliseconds.count();
}

long long int SimpleTimer::us_elapse() const {
  auto ending = steady_clock::now();
  if (stoped)
    ending = end;
  duration<long long int, std::micro> elapsed_microseconds =
    duration_cast<duration<long long int, std::micro>>(ending - begin);
  return elapsed_microseconds.count();
}

long long int SimpleTimer::ns_elapse() const {
  auto ending = steady_clock::now();
  if (stoped)
    ending = end;
  duration<long long int, std::nano> elapsed_nanoseconds =
    duration_cast<duration<long long int, std::nano>>(ending - begin);
  return elapsed_nanoseconds.count();
}

