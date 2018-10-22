/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "my_clock.h"
#include "periodic_thread.h"

#include <chrono>
#include <iostream>
#include <ctime>
#include <iomanip>

#include <vector>


using namespace std::chrono_literals;
int main()
{
#if 0
    dddwgc::periodic_thread<dddwgc::periodic_thread_flag::Suspendable, my_clock> p1{1s, [](){
#else
    dddwgc::periodic_thread<dddwgc::periodic_thread_stop_mode::Stoppable,
                            dddwgc::periodic_thread_query_mode::StatsBasicCustomClock,
                            dddwgc::periodic_thread_suspend_mode::Suspendable,
                            dddwgc::periodic_thread_yield_mode::TwiceIfNeeded,
                            dddwgc::periodic_thread_detach_mode::NotDetachable,
                            my_clock> p1{1s, [](){
#endif
        static int cnt = 0;
        auto now_c = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::now());
        std::cout << std::put_time(std::localtime(&now_c), "%F %T") << " - Lambda [1] called " << ++cnt << " times" << std::endl;
        return (cnt < 10);
    }};
    std::cout << "p1's id: " << p1.get_id() << std::endl;

//     {
//         dddwgc::periodic_thread<dddwgc::periodic_thread_flag::Detachable> p2{1s, [](){
//             static int cnt = 0;
//             auto now_c = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::now());
//             std::cout << std::put_time(std::localtime(&now_c), "%F %T") << " - Lambda [2] called " << ++cnt << " times" << std::endl;
//             return (cnt < 20);
//         }};
//         std::cout << "p2's id: " << p2.get_id() << std::endl;
// 
//         p2.detach();
//     }
// 
//     auto p3func = [&, v = std::vector{1,2,3}](){
//         static int cnt = 0;
//         auto now_c = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::now());
//         std::cout << std::put_time(std::localtime(&now_c), "%F %T") << " - Lambda [3] called " << ++cnt << " times, v[0] = " << v[0] << std::endl;
//         // this thread cannot be stopped from inside
//     };
//     auto p3period = 1s;
//     dddwgc::periodic_thread<dddwgc::periodic_thread_flag::Minimal> p3{p3period, p3func};
//     p3func(); // Check that p3func was not moved
//     std::cout << "p3period.count() = " << p3period.count() << std::endl; // Check that p3period was not moved
// //     p3.ask_to_suspend(); // should fail to build


    std::this_thread::sleep_for(10s);
//     p1.ask_to_suspend();
    std::this_thread::sleep_for(5s);
//     p1.ask_to_resume();
    std::this_thread::sleep_for(10s);

//     p1.ask_to_stop();
//     p3.ask_to_stop();

    p1.join();
//     p3.join();

//     auto x = dddwgc::periodic_thread_flag::Detachable | dddwgc::periodic_thread_flag::Suspendable;
}
