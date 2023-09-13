//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#pragma once

#include <spdlog/spdlog.h>

namespace Sierra
{

    class SIERRA_API Logger
    {
    public:
        /* --- CONSTRUCTORS --- */
        static void Initialize(const String &applicationName);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static SharedPtr<spdlog::logger>& GetEngineLogger() { return engineLogger; }
        [[nodiscard]] inline static SharedPtr<spdlog::logger>& GetApplicationLogger() { return applicationLogger; }

    private:
        inline static SharedPtr<spdlog::logger> engineLogger = nullptr;
        inline static SharedPtr<spdlog::logger> applicationLogger = nullptr;

    };

}

#if SR_ENABLE_LOGGING
    #define SR_INFO_IF(EXPRESSION, ...) if (EXPRESSION) { ::Sierra::Logger::GetEngineLogger()->info(__VA_ARGS__); }
    #define SR_WARNING_IF(EXPRESSION, ...) if (EXPRESSION) { ::Sierra::Logger::GetEngineLogger()->warn(__VA_ARGS__); }
    #define SR_ERROR_IF(EXPRESSION, ...) if (EXPRESSION) { ::Sierra::Logger::GetEngineLogger()->error(__VA_ARGS__); throw std::runtime_error("Program execution failed miserably!"); }

    #define SR_INFO(...) SR_INFO_IF(true, __VA_ARGS__)
    #define SR_WARNING(...) SR_WARNING_IF(true, __VA_ARGS__)
    #define SR_ERROR(...) SR_ERROR_IF(true, __VA_ARGS__)

#else
    #define SR_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #define SR_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #define SR_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)

    #define SR_INFO(...)
    #define SR_WARNING(...)
    #define SR_ERROR(...)

#endif

#if SR_ENABLE_LOGGING
    #define APP_INFO_IF(EXPRESSION, ...) if (EXPRESSION) { ::Sierra::Logger::GetApplicationLogger()->info(__VA_ARGS__); }
    #define APP_WARNING_IF(EXPRESSION, ...) if (EXPRESSION) { ::Sierra::Logger::GetApplicationLogger()->warn(__VA_ARGS__); }
    #define APP_ERROR_IF(EXPRESSION, ...) if (EXPRESSION) { ::Sierra::Logger::GetApplicationLogger()->error(__VA_ARGS__); throw std::runtime_error("Program execution failed miserably!"); }

    #define APP_INFO(...) APP_INFO_IF(true, __VA_ARGS__)
    #define APP_WARNING(...) APP_WARNING_IF(true, __VA_ARGS__)
    #define APP_ERROR(...) APP_ERROR_IF(true, __VA_ARGS__);

#else
    #define APP_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #define APP_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #define APP_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)

    #define APP_INFO(...)
    #define APP_WARNING(...)
    #define APP_ERROR(...)

#endif