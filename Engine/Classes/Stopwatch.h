//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <chrono>

namespace Sierra::Engine::Classes
{
    class Stopwatch
    {
    public:
        /// Sets the elapsed time to zero.
        void Reset();

        /// Returns the time the stopwatch has been running in milliseconds.
        int GetElapsedMilliseconds();

        /// Returns the time the stopwatch has been running in seconds.
        int GetElapsedSeconds();

        Stopwatch();
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };
}
