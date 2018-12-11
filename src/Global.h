#pragma once

#include <cassert>
#include <cmath>
#include <iostream>
#include <random>
#include <sys/time.h>
#include <sys/resource.h>

#define DEBUG_PRINT 0

#if DEBUG_PRINT
#define DPRINT(x) { std::cout << x << std::endl; }
#else
#define DPRINT(x) 
#endif

#define ERR(x) { std::cout << x << std::endl; exit(10); }

using float4 = float;
using float8 = double;
using sint2  = short;
using sint4  = int;
using sint8  = long long int;

static_assert(sizeof(float4) == 4, "size problem");
static_assert(sizeof(sint2) == 2, "size problem");
static_assert(sizeof(sint4) == 4, "size problem");
static_assert(sizeof(sint8) == 8, "size problem");

using RNG = std::mt19937;
using fp_t = float4; // for world geometry

template <typename T>
constexpr T square(T x) { return x*x; }

struct Vec2
{
  fp_t x, y;

  Vec2(fp_t x_ = 0, fp_t y_ = 0)
    : x(x_), y(y_)
  {
  }
  
  constexpr fp_t dist2(const Vec2 &v) const {
    return square(x-v.x)+square(y-v.y);
  }

  Vec2 sub(const Vec2 &v) const {
    return Vec2{x-v.x, y-v.y};
  }

  void scale(fp_t s) {
    x *= s;
    y *= s;
  }
};

inline std::ostream &operator<<(std::ostream &os, const Vec2 &v)
{
  os << v.x << " " << v.y;
  return os;
}

struct Timer
{
  enum Mode { CPU=0, WALLCLOCK };

  Mode mode;
  sint8 micros;

  Timer(Mode m = CPU)
    : mode(m)
  {
    set();
  }

  void set()
  {
    if (mode == CPU) {
      struct rusage ru;
      
      getrusage(RUSAGE_SELF, &ru);
      micros = ru.ru_utime.tv_sec * 1'000'000LL + ru.ru_utime.tv_usec;

    } else {

      timeval tv;
      
      gettimeofday(&tv, nullptr);
      micros = tv.tv_sec * 1'000'000LL + tv.tv_usec;
    }
  }

  // return difference in micros 
  sint8 diff(const Timer &t) const
  {
    return micros - t.micros;
  }
};


struct TimeStats
{
  TimeStats()
    : totalMicros(0), steps(0)
  { }
  
  void update(sint8 micros)
  {
    totalMicros += micros;
    ++steps;
  }
  
  double avgMillis() const
  {
    if (!steps) { return 0; }
    return (double)totalMicros / 1000.0 / steps;
  }
  
  sint8 totalMicros;
  int steps;
};
  
