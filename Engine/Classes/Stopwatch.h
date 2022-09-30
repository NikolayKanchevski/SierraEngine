//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <chrono>

namespace Sierra::Engine::Classes
{
    /// \brief A class that imitates a stopwatch functionality and can be used to detect how much time takes a block of code to get executed.
    class Stopwatch
    {
    public:
        /// \brief Sets the elapsed time to zero.
        void Reset();

        /// \brief Returns the time the stopwatch has been running in milliseconds.
        int GetElapsedMilliseconds();

        /// \brief Returns the time the stopwatch has been running in seconds.
        int GetElapsedSeconds();

        Stopwatch();
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };
}
