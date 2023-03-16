//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

namespace Sierra::Engine::Classes
{
    /// @brief Contains all kinds of time-related values. Some common ones are - FPS, delta time, etc.
    class Time
    {
    public:
        /* --- POLLING METHODS --- */
        /// Updates the values every frame.
        static void Update();

        /* --- GETTER METHODS --- */
        /// @brief Current FPS of the application. Measured per frame.
        [[nodiscard]] inline static int const GetFPS() { return FPS; }

        /// @brief Time since last frame. Used for <a href="https://www.construct.net/en/tutorials/delta-time-framerate-2">framerate independence</a>.
        [[nodiscard]] inline static float const GetDeltaTime() { return deltaTime; }

        /// @brief Time in seconds since the program has started. It is never lowered and is increased by <see cref="deltaTime"/> every frame.
        [[nodiscard]] inline static float const GetUpTime() { return upTime; }

    private:
        static inline uint FPS = 0;
        static inline float deltaTime = 0;
        static inline float upTime = 0;
        static inline double lastFrameTime = 0;

    };
}
