#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

#include "core/device.h"
#include "core/clock.h"

namespace EaterEmulator::core 
{
    class ClockedDevice : public Device, public core::ClockObserver {
    public:
        ClockedDevice(Bus& bus, uint16_t offset = 0) 
            : Device(bus, offset), _cpuThread(), _stateQueue(), _queueMutex(), _cv()
        {
            // Start the background thread
            _cpuThread = std::jthread([this](std::stop_token stop_token) {
                cpuLoop(stop_token);
            });
        }

        ~ClockedDevice() {
            // Stop the background thread
            stop();
        }

        void onClockStateChange(core::State state) override {
            // Queue the clock state change to be handled in the background thread
            {
                std::lock_guard<std::mutex> lock(_queueMutex);
                _stateQueue.push(state);
            }
            _cv.notify_one();
        }

        void stop() {
            if (_cpuThread.joinable()) {
                _cpuThread.request_stop();
                _cv.notify_all();
            }
        }

    protected:
        // Override this method to handle clock state changes
        virtual void handleClockStateChange(core::State state) = 0;

    private:
        void cpuLoop(std::stop_token stop_token) {
            while (!stop_token.stop_requested()) {
                core::State state;
                
                // Wait for a clock state change
                {
                    std::unique_lock<std::mutex> lock(_queueMutex);
                    _cv.wait(lock, [this, &stop_token] { 
                        return !_stateQueue.empty() || stop_token.stop_requested(); 
                    });
                    
                    if (stop_token.stop_requested()) break;
                    
                    state = _stateQueue.front();
                    _stateQueue.pop();
                }
                
                // Handle the clock state change in the background thread
                handleClockStateChange(state);
            }
        }

        std::jthread _cpuThread;
        std::queue<core::State> _stateQueue;
        std::mutex _queueMutex;
        std::condition_variable _cv;
    };
}