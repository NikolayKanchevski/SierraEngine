//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#include "Logger.h"

#if SR_ENABLE_LOGGING
    #if !SR_PLATFORM_ANDROID
        #include <spdlog/sinks/stdout_color_sinks.h>
    #else
        #include <spdlog/sinks/android_sink.h>
    #endif
#endif

namespace Sierra
{

    void Logger::Initialize(const std::string &applicationName)
    {
        #if SR_ENABLE_LOGGING
            // Set global logging pattern [Time / Logger Name / Arguments]
            spdlog::set_pattern("%^[%T] %n: %v%$");

            // Create loggers
            #if !SR_PLATFORM_ANDROID
                engineLogger = spdlog::stderr_color_mt("Sierra API");
                engineLogger->set_level(spdlog::level::info);
                applicationLogger = spdlog::stderr_color_mt(applicationName);
                applicationLogger->set_level(spdlog::level::info);
            #else
                engineLogger = spdlog::android_logger_mt("Sierra API");
                engineLogger->set_level(spdlog::level::info);
                applicationLogger = spdlog::android_logger_mt(applicationName);
                applicationLogger->set_level(spdlog::level::info);
            #endif
        #endif
    }

}