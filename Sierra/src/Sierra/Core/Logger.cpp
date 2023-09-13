//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#include "Logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Sierra
{

    void Logger::Initialize(const String &applicationName)
    {
        // Set global logging pattern [Time / Logger Name / Arguments]
        spdlog::set_pattern("%^[%T] %n: %v%$");

        // Create loggers
        engineLogger = spdlog::stderr_color_mt("Sierra API");
        engineLogger->set_level(spdlog::level::info);
        applicationLogger = spdlog::stderr_color_mt(applicationName);
        applicationLogger->set_level(spdlog::level::info);
    }

}