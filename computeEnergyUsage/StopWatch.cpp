
/*
 * File:   StopWatch.cpp
 * Author: KjellKod
 * From: https://github.com/KjellKod/StopWatch
 *
 * Created on 2014-02-07
 *
 *
 *Timing utility, wraps C++11 std::chrono in an easy to use StopWatch utility
 *Originally published at: kjellkod.wordpress.com/2012/02/06/exploring-c11-part1-time/
 *
 */

#include "StopWatch.hpp"
StopWatch::StopWatch() : mStart(clock::now()) { //Instantiating this object starts the clock.
    static_assert(std::chrono::steady_clock::is_steady, "Serious OS/C++ library issues. Steady clock is not steady");
    // FYI:  This would fail  static_assert(std::chrono::high_resolution_clock::is_steady(), "High Resolution Clock is NOT steady on CentOS?!");
}

StopWatch::StopWatch(const StopWatch& other): mStart(other.mStart) {
}

/// @return StopWatch::StopWatch&  - assignment operator.
StopWatch& StopWatch::operator=(const StopWatch& rhs) {
    mStart = rhs.mStart;
    return *this;
}
/// @return the elapsed nanoseconds since start
uint64_t StopWatch::ElapsedNs() const {
    return std::chrono::duration_cast<nanoseconds>(clock::now()-mStart).count(); //Silly, whimsical, precision added by cjc on 2018-06-10T14:03:57
}
/// @return the elapsed microseconds since start
uint64_t StopWatch::ElapsedUs() const {
    return std::chrono::duration_cast<microseconds>(clock::now() - mStart).count();
}

/// @return the elapsed milliseconds since start
uint64_t StopWatch::ElapsedMs() const {
    return std::chrono::duration_cast<milliseconds>(clock::now() - mStart).count();
}

/// @return the elapsed seconds since start
uint64_t StopWatch::ElapsedSec() const {
    return std::chrono::duration_cast<seconds>(clock::now() - mStart).count();
}
/**
 * Resets the start point
 * @return the updated start point
 */
std::chrono::steady_clock::time_point StopWatch::Restart() {
    mStart = clock::now();
    return mStart;
}
