/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef my_clock_h
#define my_clock_h

#include <chrono>
#include <thread>

constexpr double real_time_factor = 3.0;

struct my_clock
{
    typedef std::chrono::nanoseconds duration;
    typedef duration::rep rep;
    typedef duration::period period;
    typedef std::chrono::time_point<my_clock, duration> time_point;

    static constexpr bool is_steady = false;

    static time_point now() noexcept {
        auto real = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        auto fake = static_cast<rep>((real - 1'500'000'000'000'000'000) * real_time_factor);
        return time_point(duration(fake));
    }
};

namespace std {
namespace this_thread {
template <>
void sleep_until<my_clock, my_clock::duration>(const std::chrono::time_point<my_clock, my_clock::duration>& __atime)
{
    auto __now = my_clock::now();
    while (__now < __atime)
    {
        auto cnt = static_cast<my_clock::rep>((__atime - __now).count() / real_time_factor);
        sleep_for(my_clock::duration(cnt));
        __now = my_clock::now();
    }
}
} // namespace this_tread
} // namespace std

#endif // my_clock_h
