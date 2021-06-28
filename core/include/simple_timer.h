#pragma once

#include <chrono>

class SimpleTimer {
 public:
  SimpleTimer() { start(); }
  virtual ~SimpleTimer() {}

  void start();
  double stop();

  double elapse() const;
  long long int ms_elapse() const;
  long long int us_elapse() const;
  long long int ns_elapse() const;

 private:
  bool stoped;
  std::chrono::time_point<std::chrono::steady_clock> begin;
  std::chrono::time_point<std::chrono::steady_clock> end;
};
