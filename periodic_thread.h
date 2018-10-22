/*
 * Copyright (C) 2018 Daniele E. Domenichelli <ddomenichelli@drdanz.it>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#ifndef periodic_thread_h
#define periodic_thread_h

#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <utility>
#include <cstdint>

#include "flags/flags.hpp"

#include <iostream>
#include "tracer.h"

namespace dddwgc {
#ifdef USE_V1_DEFAULT
inline
#else
#error Define USE_V1_DEFAULT
#endif
namespace _V1 {



// === BEGIN stop mode ===
enum class periodic_thread_stop_mode
{
    NotStoppable,
    Stoppable,
};

template<periodic_thread_stop_mode stoppable_mode>
struct stop_s {};

template<>
struct stop_s<periodic_thread_stop_mode::Stoppable>
{
    std::atomic_flag continueFlag = ATOMIC_FLAG_INIT;
};
// === END stop mode ===



// === BEGIN query mode ===
enum class periodic_thread_query_mode
{
    NotQueryable,
    Queryable,
    StatsBasic,
    StatsBasicCustomClock,
    StatsAdvanced,
    StatsAdvancedCustomClock,
};

template<typename Clock, periodic_thread_query_mode query_mode>
struct query_s {};

template<typename Clock>
struct query_s<Clock, periodic_thread_query_mode::Queryable>
{
    std::atomic_bool runningFlag = false;
};

template<typename Clock>
struct query_s<Clock, periodic_thread_query_mode::StatsBasic>
        : query_s<Clock, periodic_thread_query_mode::Queryable>
{
    std::mutex statsMutex;

    unsigned long int count = 0L; //number of iterations from last reset
    unsigned long int estPIt = 0L; //number of useful iterations for period estimation

    std::chrono::high_resolution_clock::time_point currentStart;
    std::chrono::high_resolution_clock::time_point previousStart;

    typename Clock::duration totalUsed;        //total time taken iterations
    typename Clock::duration totalT;           //time bw run, accumulated
    double sumTSq = 0.0;           //cumulative sum sq of estimated period dT
    double sumUsedSq = 0.0;        //cumulative sum sq of estimated thread tun
    double adaptedPeriod = 0.0;
};

template<typename Clock>
struct query_s<Clock, periodic_thread_query_mode::StatsBasicCustomClock>
        : query_s<Clock, periodic_thread_query_mode::StatsBasic>
{
    typename Clock::time_point currentStart_custom;
    typename Clock::time_point previousStart_custom;

    typename Clock::duration totalUsed_custom;        //total time taken iterations
    typename Clock::duration totalT_custom;           //time bw run, accumulated
    double sumTSq_custom = 0.0;           //cumulative sum sq of estimated period dT
    double sumUsedSq_custom = 0.0;        //cumulative sum sq of estimated thread tun
    double adaptedPeriod_custom = 0.0;
};

// === END query mode ===



// === BEGIN suspend mode ===
enum class periodic_thread_suspend_mode
{
    NotSuspendable,
    Suspendable,
};

template<periodic_thread_suspend_mode suspend_mode>
struct suspend_s {};

template<>
struct suspend_s<periodic_thread_suspend_mode::Suspendable>
{
    bool suspended = false;
    std::mutex suspendMutex;
    std::condition_variable resumeCondition;
};
// === END suspend mode ===



// === BEGIN yield mode ===
enum class periodic_thread_yield_mode
{
    No,
    Once,
    TwiceIfNeeded,
    Twice,
};
// === END yield mode ===



/*
template<periodic_thread_stop_mode stop_m,
         periodic_thread_query_mode query_m,
         periodic_thread_suspend_mode suspend_m,
         periodic_thread_yield_mode yield_m>
struct periodic_thread_mode
{
    static constexpr periodic_thread_stop_mode stop_mode = stop_m;
    static constexpr periodic_thread_query_mode query_mode = query_m;
    static constexpr periodic_thread_suspend_mode suspend_mode = suspend_m;
    static constexpr periodic_thread_yield_mode yield_mode = yield_m;
};
*/



template<typename Clock,
         periodic_thread_stop_mode stop_m,
         periodic_thread_query_mode query_m,
         periodic_thread_suspend_mode suspend_m,
         periodic_thread_yield_mode yield_m>
struct periodic_thread_s
        : stop_s<stop_m>
        , query_s<Clock, query_m>
        , suspend_s<suspend_m>
{
    typename Clock::duration period;
};



// === BEGIN detach mode ===
enum class periodic_thread_detach_mode
{
    NotDetachable,
    Detachable,
    Detached,
};
// A struct that holds either an object (deleted on destruction), or a
// shared_ptr that is passed to the loop function, in order to allow to detach
template<typename Clock,
         periodic_thread_stop_mode stop_m,
         periodic_thread_query_mode query_m,
         periodic_thread_suspend_mode suspend_m,
         periodic_thread_yield_mode yield_m,
         periodic_thread_detach_mode detach_mode>
struct detach_s {
    using type = periodic_thread_s<Clock, stop_m, query_m, suspend_m, yield_m>;
    using ptr_type = periodic_thread_s<Clock, stop_m, query_m, suspend_m, yield_m>* const;
    type _s;
    ptr_type s = &_s;
};

// This is a shared_ptr because the caller could call detach and destroy
// the periodic_thread object
template<typename Clock,
         periodic_thread_stop_mode stop_m,
         periodic_thread_query_mode query_m,
         periodic_thread_suspend_mode suspend_m,
         periodic_thread_yield_mode yield_m>
struct detach_s<Clock, stop_m, query_m, suspend_m, yield_m, periodic_thread_detach_mode::Detachable> {
    using type = periodic_thread_s<Clock, stop_m, query_m, suspend_m, yield_m>;
    using ptr_type = const std::shared_ptr<type>;
    ptr_type s = std::make_shared<type>();
};
// === END detach mode ===



template<periodic_thread_stop_mode stop_m,
         periodic_thread_query_mode query_m,
         periodic_thread_suspend_mode suspend_m,
         periodic_thread_yield_mode yield_m,
         periodic_thread_detach_mode detach_m,
         typename Clock = std::chrono::high_resolution_clock>
class periodic_thread : detach_s<Clock, stop_m, query_m, suspend_m, yield_m, detach_m>
{
    // Ensure that clock is a clock
    // FIXME check the other members
    static_assert(std::is_nothrow_invocable_r_v<typename Clock::time_point, decltype(Clock::now)>,
        "Clock should have a 'Clock::time_point now() noexcept' method");

    using status_s = detach_s<Clock, stop_m, query_m, suspend_m, yield_m, detach_m>;

    std::thread t;

public:
    template <typename Duration, typename Func>
    periodic_thread(const Duration& period, Func&& f)
    {
        /* FIXME static_assert(Duration is a duration); */

        // Ensure that Func is invocable and that returns a bool
        static_assert(std::is_invocable_r_v<void, Func>
                   || std::is_invocable_r_v<bool, Func>,
            "Func should be a callable that is invokable without parameters and returns either void or bool");

        static_assert(std::is_invocable_r_v<bool, Func>
                   || stop_m == periodic_thread_stop_mode::Stoppable
                   || detach_m == periodic_thread_detach_mode::Detachable,
            "No way to stop the thread. Options: 1) make it stoppable, 2) make it detachable or 3) return a bool from the main loop");

        // FIXME should be during initialization?
        status_s::s->period = std::move(std::chrono::duration_cast<typename Clock::duration>(period));

#define PRINT_STRUCT_SIZE
#ifdef PRINT_STRUCT_SIZE
        std::cout << "sizeof(int): " << sizeof(int) << std::endl;
        std::cout << "sizeof(*this): " << sizeof(*this) << std::endl;
        std::cout << " sizeof(std::thread): " << sizeof(std::thread) << std::endl;
        std::cerr << " sizeof(status_s) = " << sizeof(status_s) << std::endl;
        std::cerr << "  sizeof(typename status_s::ptr_type) = " << sizeof(typename status_s::ptr_type) << std::endl;
        if constexpr (detach_m == periodic_thread_detach_mode::NotDetachable) {
            std::cerr << "  sizeof(typename status_s::type) = " << sizeof(typename status_s::type) << std::endl;
            std::cerr << "   sizeof(typename Clock::duration) = " << sizeof(typename Clock::duration) << std::endl;
            std::cerr << "   sizeof(stop_s<stop_m>) = " << sizeof(stop_s<stop_m>) << std::endl;
            std::cerr << "   sizeof(query_s<query_m>) = " << sizeof(query_s<Clock, query_m>) << std::endl;
            std::cerr << "   sizeof(suspend_s<suspend_m>) = " << sizeof(suspend_s<suspend_m>) << std::endl;
        }
//         std::cerr << "   sizeof(yield_s<yield_m>) = " << sizeof(yield_s<yield_m>) << std::endl;
#endif

        // If the thread is stoppable, enable the continueFlag
        if constexpr (stop_m == periodic_thread_stop_mode::Stoppable) {
            status_s::s->continueFlag.test_and_set(std::memory_order_relaxed);
        }

        t = std::move(std::thread([s = status_s::s,
                                   f = std::forward<Func>(f)]{

            // Set the runnning flag before entering the loop
            if constexpr (query_m != periodic_thread_query_mode::NotQueryable) {
                s->runningFlag.store(true, std::memory_order_relaxed);
            }

            // Infinite loop, since non-stoppable threads don't have a flag to
            // check
            while (true) {

                // Check the continueFlag and eventually leave the loop
                if constexpr (stop_m == periodic_thread_stop_mode::Stoppable) {
                    if(!s->continueFlag.test_and_set(std::memory_order_relaxed)) {
                        break;
                    }
                }

                // Check if it should be suspended
                if constexpr (suspend_m == periodic_thread_suspend_mode::Suspendable) {
                    {
                        // Enclosed in a scope to ensure the that the mutex is
                        // unlocked as soon as possible
                        std::unique_lock<std::mutex> lock(s->suspendMutex);
                        s->resumeCondition.wait(lock, [&]{ return !s->suspended; });
                    }

                    // Check the continueFlag again that might have been modified
                    // while suspended
                    if constexpr (stop_m == periodic_thread_stop_mode::Stoppable) {
                        if (!s->continueFlag.test_and_set(std::memory_order_relaxed)) {
                            break;
                        }
                    }
                }

                // Take the starting time
                auto start = Clock::now();

                // Take start statistics
                if constexpr (query_m == periodic_thread_query_mode::StatsBasic
                           || query_m == periodic_thread_query_mode::StatsBasicCustomClock) {

                    // Take the real time before acquiring the mutex (no need to
                    // take the custom clock time, we use "start" instead)
                    auto start_system = std::chrono::high_resolution_clock::now();

                    std::lock_guard(s->statsMutex);

                    // if (scheduleReset)
                    //    _resetStat();

                    // Save current start
                    s->currentStart = std::move(start_system);
                    if constexpr (query_m == periodic_thread_query_mode::StatsBasicCustomClock) {
                        s->currentStart_custom = start;
                    }

                    if (s->count > 0) {
                        auto dT = s->currentStart - s->previousStart;
                        s->totalT += dT;

                        auto dT_d = std::chrono::duration_cast<std::chrono::duration<double>>(dT).count();
                        s->sumTSq += dT_d * dT_d;

                        std::cerr << "dT.count() = " << dT.count() << ", dT_d = " << dT_d << ", dT_d^2 = " << dT_d*dT_d << std::endl;

                        if constexpr (query_m == periodic_thread_query_mode::StatsBasicCustomClock) {
                            auto dT_custom = s->currentStart_custom - s->previousStart_custom;
                            s->totalT_custom += dT_custom;

                            auto dT_custom_d = std::chrono::duration_cast<std::chrono::duration<double>>(dT_custom).count();
                            s->sumTSq_custom += dT_custom_d * dT_custom_d;

                            std::cerr << "dT_custom.count() = " << dT_custom.count() << ", dT_custom_d = " << dT_custom_d << ", dT_custom_d^2 = " << dT_custom_d*dT_custom_d << std::endl;
                        }

//                         if (s->adaptedPeriod < 0) {
//                             s->adaptedPeriod = 0;
//                         }

                        ++(s->estPIt);

                        fprintf(stderr, "count = %ld - REAL:   sumTSq %fd, totalT %ld\n", s->count, dT, s->sumTSq, s->totalT.count());
                        if constexpr (query_m == periodic_thread_query_mode::StatsBasicCustomClock) {
                            fprintf(stderr, "              CUSTOM: sumTSq %fd, totalT %ld\n", s->sumTSq_custom, s->totalT_custom.count());
                        }
                    }
                }

                // Invoke the callable
                if constexpr(std::is_invocable_r_v<bool, Func>) {
                    // Check the return value and exit the loop if the callable
                    // returned false
                    if (!f()) {
                        break;
                    }
                } else if constexpr(std::is_invocable_r_v<void, Func>) {
                    // No need to check the return value, the callable can not
                    // exit the loop unless the thread is stopped from outside
                    f();
                }

                // Take end statistics
                if constexpr (query_m == periodic_thread_query_mode::StatsBasic
                           || query_m == periodic_thread_query_mode::StatsBasicCustomClock) {
                    // Take the real time before acquiring the mutex
                    auto end_system = std::chrono::high_resolution_clock::now();
                    auto end_custom = Clock::now();

                    std::lock_guard(s->statsMutex);

                    auto elapsed = end_system - s->currentStart;
                    s->totalUsed += elapsed;

                    auto elapsed_d = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed).count();
                    s->sumUsedSq += elapsed_d * elapsed_d;

                    s->previousStart = std::move(s->currentStart);
                    fprintf(stderr, "REAL:   elapsed = %lf, totalUsed = %ld, sumUsedSq %lf\n", elapsed_d, s->totalUsed.count(), s->sumUsedSq);

                    if constexpr (query_m == periodic_thread_query_mode::StatsBasicCustomClock) {
                        auto elapsed_custom = end_custom - s->currentStart_custom;
                        s->totalUsed_custom += elapsed_custom;

                        auto elapsed_custom_d = std::chrono::duration_cast<std::chrono::duration<double>>(elapsed_custom).count();
                        s->sumUsedSq_custom += elapsed_custom_d * elapsed_custom_d;

                        s->previousStart_custom = std::move(s->currentStart_custom);
                        fprintf(stderr, "CuSTOM: elapsed = %lf, totalUsed = %ld, sumUsedSq %lf\n", elapsed_custom_d, s->totalUsed_custom.count(), s->sumUsedSq_custom);
                    }

                    ++(s->count);
                }

                // Do not wait for the sleep to return if the thread was asked
                // to stop
                if constexpr (stop_m == periodic_thread_stop_mode::Stoppable) {
                    if (!s->continueFlag.test_and_set(std::memory_order_relaxed)) {
                        break;
                    }
                }

                // Reschedule the thread. This will help in case the duration
                // of the step is greater than the period.
                // Please read the warning below.
                if constexpr (yield_m != periodic_thread_yield_mode::No) {
                    std::this_thread::yield();
                }

                // Linux default scheduler (and probably some others) use a FIFO
                // queue for ready and waiting threads (Windows and macOS are
                // not influenced by this issue).
                // When a thread yields, the threads that are on wait queue and
                // are now ready, are moved to the ready queue, and a thread
                // that was already on the ready queue is started.
                // Since current thread is already in the ready queue, the
                // threads that are waiting for some resource held by this
                // thread are behind current thread, therefore this thread is
                // started again.
                // This in some cases can cause starvation, yielding twice will
                // push current thread at the back of the queue, ensuring that
                // the other threads run first.
                // Hence the TwiceIfNeeded flag. The Twice flag is for testing,
                // it should not be useful in practice.
                //
                // WARNING: These flags are hardly needed in practice, if you
                //          need to use use one of these two flags to get your
                //          code working, you probably have some issues in your
                //          code, probably the critical sections are too big,
                //          and the duration of the step takes more than the
                //          requested time.
                if constexpr (yield_m == periodic_thread_yield_mode::Twice
#if defined(__linux__) // FIXME this check should be on the scheduler, not on
                       //       the operating system
                           || yield_m == periodic_thread_yield_mode::TwiceIfNeeded
#endif
                ) {
                    std::this_thread::yield();
                }

                // Sleep until the next execution scheduled
                std::this_thread::sleep_until(start + s->period);
            }

            // Clear the runnning flag before after exiting the loop
            if constexpr (query_m != periodic_thread_query_mode::NotQueryable) {
                s->runningFlag.store(false, std::memory_order_relaxed);
            }
        }));
    }

    ~periodic_thread() = default;

    // non copiable
    periodic_thread(const periodic_thread&) = delete;
    periodic_thread& operator=(const periodic_thread&) = delete;

    // movable
    periodic_thread(periodic_thread&&) = default;
    periodic_thread& operator=(periodic_thread&&) = default;

    template <periodic_thread_stop_mode mode = stop_m,
              typename = typename std::enable_if_t<mode == periodic_thread_stop_mode::Stoppable>>
    void ask_to_stop()
    {
        status_s::s->continueFlag.clear(std::memory_order_relaxed);
    }

    template <periodic_thread_query_mode mode = query_m,
              typename = typename std::enable_if_t<mode != periodic_thread_query_mode::NotQueryable>>
    bool is_running()
    {
        return status_s::s->runningFlag.load(std::memory_order_relaxed);
    }

    template <periodic_thread_suspend_mode mode = suspend_m,
              typename = typename std::enable_if_t<mode == periodic_thread_suspend_mode::Suspendable>>
    bool is_suspended()
    {
        std::lock_guard<std::mutex> lock(status_s::s->suspendMutex);
        return status_s::s->suspended;
    }

    template <periodic_thread_suspend_mode mode = suspend_m,
              typename = typename std::enable_if_t<mode == periodic_thread_suspend_mode::Suspendable>>
    void ask_to_suspend()
    {
        std::lock_guard<std::mutex> lock(status_s::s->suspendMutex);
        status_s::s->suspended = true;
    }

    template <periodic_thread_suspend_mode mode = suspend_m,
              typename = typename std::enable_if_t<mode == periodic_thread_suspend_mode::Suspendable>>
    void ask_to_resume()
    {
        std::lock_guard<std::mutex> lock(status_s::s->suspendMutex);
        status_s::s->suspended = false;
        status_s::s->resumeCondition.notify_one();
    }

    typename Clock::duration get_period()
    {
        return status_s::s->period;
    }

    void set_period(typename Clock::duration period)
    {
        status_s::s->period = std::move(period);
    }

    // std::thread API
    typedef std::thread::id id;
    typedef std::thread::native_handle_type native_handle_type;

    id get_id() const noexcept
    {
        return t.get_id();
    }

    native_handle_type native_handle()
    {
        return t.native_handle();
    }

    void join()
    {
        t.join();
    }

    template <periodic_thread_detach_mode mode = detach_m,
              typename = typename std::enable_if_t<mode == periodic_thread_detach_mode::Detachable>>
    void detach()
    {
        t.detach();
    }

    bool joinable()
    {
        return t.joinable();
    }
};

} // namespace _V1
} // namespace dddwgc

#endif // periodic_thread_h
