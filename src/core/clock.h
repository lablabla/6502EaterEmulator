#pragma once

#include "core/defines.h"

#include <condition_variable>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <algorithm>

namespace EaterEmulator::core
{

    class ClockObserver;

    template<auto Hz>
    class Clock
    {
    public:

        static constexpr auto frequency = Hz;
        static constexpr auto period = std::chrono::nanoseconds{1'000'000'000 / Hz};
        static constexpr auto half_period = period / 2;

        Clock() = default;
        ~Clock() = default;

        // Non-copyable, movable
        Clock(const Clock&) = delete;
        Clock& operator=(const Clock&) = delete;
        Clock(Clock&&) = default;
        Clock& operator=(Clock&&) = default;

        void start() {
            if (!_running.exchange(true, std::memory_order_acq_rel)) {
                _clockThread = std::jthread([this](std::stop_token stop_token) {
                    clockLoop(stop_token);
                });
            }
        }

        void stop() {
            if (_running.exchange(false, std::memory_order_acq_rel)) {
                _clockThread.request_stop();
                _cv.notify_all();
            }
        }
        
        void registerObserver(ClockObserver* observer) {
            std::lock_guard lock(_observerMutex);
            _observers.push_back(observer);
        }
        
        void unregisterObserver(ClockObserver* observer) {
            std::lock_guard lock(_observerMutex);
            auto it = std::ranges::find(_observers, observer);
            if (it != _observers.end()) {
                _observers.erase(it);
            }
        }
        
    private:
    void clockLoop(std::stop_token stop_token) {
            using namespace std::chrono;
            
            auto next_tick = steady_clock::now();
            
            while (!stop_token.stop_requested()) {
                next_tick += half_period;
                
                // Toggle state with memory ordering
                const auto current_state = _state.load(std::memory_order_acquire);
                const auto new_state = (current_state == LOW) ? HIGH : LOW;
                _state.store(new_state, std::memory_order_release);
                
                // Notify all observers
                notifyObservers(new_state);
                
                // Sleep until next tick or stop requested
                if (stop_token.stop_requested()) break;
                std::this_thread::sleep_until(next_tick);
            }
        }

        void notifyObservers(State new_state) {
            std::shared_lock lock(_observerMutex);
            
            // Use ranges for-each
            std::ranges::for_each(_observers, [new_state](auto* observer) {
                observer->onClockStateChange(new_state);
            });
        }

        std::condition_variable _cv;
        mutable std::shared_mutex _observerMutex;
        
        std::vector<ClockObserver*> _observers;
        std::atomic<State> _state{LOW};
        std::atomic<bool> _running{false};
        std::jthread _clockThread;
    };
    
    // Base class for all clock-dependent devices    
    class ClockObserver {
    public:
        virtual ~ClockObserver() = default;
        virtual void onClockStateChange(State newState) = 0;
    };
}