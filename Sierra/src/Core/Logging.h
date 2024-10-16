//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#pragma once

#if SR_ENABLE_LOGGING

#include <spdlog/spdlog.h>

namespace Sierra
{

    class SIERRA_API Logger final
    {
    public:
        /* --- GETTER METHODS --- */
        #if defined(SR_LIBRARY_IMPLEMENTATION)
            [[nodiscard]] static spdlog::logger& GetCoreLogger() noexcept;
        #endif
        [[nodiscard]] static spdlog::logger& GetApplicationLogger() noexcept;

        /* --- COPY SEMANTICS --- */
        Logger(const Logger&) = delete;
        Logger& operator=(const Logger&) = delete;

        /* --- MOVE SEMANTICS --- */
        Logger(Logger&&) = delete;
        Logger& operator=(Logger&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Logger() noexcept = default;

    private:
        friend class Application;
        static void Initialize(std::string_view applicationName);

    };

    class SIERRA_API ScopeProfiler final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ScopeProfiler(std::string_view scopeName);

        /* --- COPY SEMANTICS --- */
        ScopeProfiler(const ScopeProfiler&) = delete;
        ScopeProfiler& operator=(const ScopeProfiler&) = delete;

        /* --- MOVE SEMANTICS --- */
        ScopeProfiler(ScopeProfiler&&) = delete;
        ScopeProfiler& operator=(ScopeProfiler&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ScopeProfiler() noexcept;

    private:
        std::string scopeName;
        const std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    };

}

#endif