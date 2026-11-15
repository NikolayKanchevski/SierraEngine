//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#include "Logger.h"

#if !SR_PLATFORM_ANDROID
    #include <spdlog/sinks/stdout_color_sinks.h>
#else
    #include <spdlog/sinks/android_sink.h>
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

        // Create loggers
        #if !SR_PLATFORM_ANDROID
            coreLogger = spdlog::stderr_color_mt("Sierra API");
            applicationLogger = spdlog::stderr_color_mt(applicationName.data());
        #else
            coreLogger = spdlog::android_logger_mt("Sierra API");
            applicationLogger = spdlog::android_logger_mt(applicationName.data());
        #endif
        // Pattern [Time / Logger Name / Message], with the whole line coloured by level. Set on each logger directly
        coreLogger->set_pattern("%^[%T] %n: %v%$");
        applicationLogger->set_pattern("%^[%T] %n: %v%$");

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

}