//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <chrono>

namespace Sierra::Engine::Classes
{
    /// @brief A class that imitates a stopwatch functionality and can be used to detect how much time takes a block of code to get executed.
    class Stopwatch
    {
    public:
        /* --- CONSTRUCTORS --- */
        Stopwatch();

        /* --- SETTER FUNCTIONS --- */
        /// @brief Sets the elapsed time to zero.
        void Reset();

        /* --- GETTER FUNCTIONS --- */
        /// @brief Returns the time the stopwatch has been running in milliseconds.
        [[nodiscard]] int GetElapsedMilliseconds();

        /// @brief Returns the time the stopwatch has been running in seconds.
        [[nodiscard]] int GetElapsedSeconds();
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };
}
