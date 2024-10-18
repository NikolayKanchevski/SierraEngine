//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#include "Logging.h"

#if SR_ENABLE_LOGGING
    #if !SR_PLATFORM_ANDROID
        #include <spdlog/sinks/stdout_color_sinks.h>
    #else
        #include <spdlog/sinks/android_sink.h>
    #endif
#endif

namespace Sierra
{

    namespace
    {
        std::shared_ptr<spdlog::logger> coreLogger = nullptr;
        std::shared_ptr<spdlog::logger> applicationLogger = nullptr;
    }

    /* --- CONSTRUCTORS --- */

    void Logger::Initialize(const std::string_view applicationName)
    {
        // Set global logging pattern [Time / Logger Name / Arguments]
        spdlog::set_pattern("%^[%T] %n: %v%$");

        // Create loggers
        #if !SR_PLATFORM_ANDROID
            coreLogger = spdlog::stderr_color_mt("Sierra API");
            applicationLogger = spdlog::stderr_color_mt(applicationName.data());
        #else
            coreLogger = spdlog::android_logger_mt("Sierra API");
            applicationLogger = spdlog::android_logger_mt(applicationName.data());
        #endif
        coreLogger->set_level(spdlog::level::info);
        applicationLogger->set_level(spdlog::level::info);
    }

    /* --- GETTER METHODS --- */

    spdlog::logger& Logger::GetCoreLogger() noexcept
    {
        return *coreLogger;
    }

    spdlog::logger& Logger::GetApplicationLogger() noexcept
    {
        return *applicationLogger;
    }


    /* --- CONSTRUCTORS --- */

    ScopeProfiler::ScopeProfiler(const std::string_view scopeName)
        : scopeName(scopeName), startTime(std::chrono::high_resolution_clock::now())
    {

    }

    /* --- DESTRUCTOR --- */

    ScopeProfiler::~ScopeProfiler() noexcept
    {
        const auto endTime = std::chrono::high_resolution_clock::now();
        const uint64 duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        SR_INFO("Scope [{0}] took {1}ms to execute", scopeName, duration);
    }

}