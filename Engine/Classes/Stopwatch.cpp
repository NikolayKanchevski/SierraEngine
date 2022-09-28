//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Stopwatch.h"

namespace Sierra::Engine::Classes
{
    Stopwatch::Stopwatch()
    {
        this->startTime = std::chrono::high_resolution_clock::now();
    }

    void Stopwatch::Reset()
    {
        this->startTime = std::chrono::high_resolution_clock::now();
    }

    int Stopwatch::GetElapsedMilliseconds()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> processDuration = endTime - startTime;

        return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(processDuration).count());
    }

    int Stopwatch::GetElapsedSeconds()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> processDuration = endTime - startTime;

        return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(processDuration).count());
    }
}