//
//  StopWatch.hpp
//  multiThreadEnergyUsage
//
//  Created by Clifford Campo on 6/9/18.
//  Copyright © 2018 CliffordCampo. All rights reserved.
//

#ifndef StopWatch_hpp
#define StopWatch_hpp

#include <chrono>


class StopWatch {
public:
    typedef std::chrono::steady_clock clock;
    typedef std::chrono::nanoseconds nanoseconds;
    typedef std::chrono::microseconds microseconds;
    typedef std::chrono::milliseconds milliseconds;
    typedef std::chrono::seconds seconds;
    
    StopWatch();
    StopWatch(const StopWatch&);
    StopWatch& operator=(const StopWatch& rhs);
    uint64_t ElapsedNs() const;
    uint64_t ElapsedUs() const;
    uint64_t ElapsedMs() const;
    uint64_t ElapsedSec() const;
    
    std::chrono::steady_clock::time_point Restart();
    
private:
    clock::time_point mStart;
};
#endif /* StopWatch_hpp */
