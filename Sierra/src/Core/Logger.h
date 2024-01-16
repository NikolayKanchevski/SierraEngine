//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#pragma once

#if SR_ENABLE_LOGGING
    #include <spdlog/spdlog.h>
#endif

namespace Sierra
{

    enum class MessageSeverity
    {
        Info,
        Warning,
        Error
    };

    class SIERRA_API Logger
    {
    public:
        /* --- POLLING METHODS --- */
        template<typename... Args>
        #if SR_ENABLE_LOGGING
            constexpr static void LogMessage(const MessageSeverity messageSeverity, fmt::format_string<Args...> fmt, Args &&...args)
            {
                switch (messageSeverity)
                {
                    case MessageSeverity::Info:
                    {
                        applicationLogger->info(fmt, std::forward<Args>(args)...);
                        break;
                    }
                    case MessageSeverity::Warning:
                    {
                        applicationLogger->warn(fmt, std::forward<Args>(args)...);
                        break;
                    }
                    case MessageSeverity::Error:
                    {
                        applicationLogger->error(fmt, std::forward<Args>(args)...);
                        #if SR_ENABLE_LOGGING
                            throw std::runtime_error("Program execution failed miserably!");
                        #endif
                        break;
                    }
                }
            }
        #else
            constexpr static void LogMessage(const MessageSeverity messageSeverity, Args &&...args) { }
        #endif

        template<typename... Args>
        #if SR_ENABLE_LOGGING
            constexpr static void LogMessageIf(const MessageSeverity messageSeverity, const bool expression, fmt::format_string<Args...> fmt, Args &&...args)
            {
                if (expression) LogMessage(messageSeverity, fmt, std::forward<Args>(args)...);
            }
        #else
            constexpr static void LogMessageIf(const MessageSeverity messageSeverity, const bool expression, Args &&...args) { }
        #endif

        #if defined(SR_LIBRARY_IMPLEMENTATION)
            template<typename... Args>
            #if SR_ENABLE_LOGGING
                constexpr static void LogCoreMessage(const MessageSeverity messageSeverity, fmt::format_string<Args...> fmt, Args &&...args)
                {
                    #if !SR_ENABLE_LOGGING
                        return;
                    #endif

                    switch (messageSeverity)
                    {
                        case MessageSeverity::Info:
                        {
                            engineLogger->info(fmt, std::forward<Args>(args)...);
                            break;
                        }
                        case MessageSeverity::Warning:
                        {
                            engineLogger->warn(fmt, std::forward<Args>(args)...);
                            break;
                        }
                        case MessageSeverity::Error:
                        {
                            engineLogger->error(fmt, std::forward<Args>(args)...);
                            #if SR_ENABLE_LOGGING
                                throw std::runtime_error("Program execution failed miserably!");
                            #endif
                            break;
                        }
                    }
                }
            #else
                constexpr static void LogCoreMessage(const MessageSeverity messageSeverity, Args &&...args) { }
            #endif

            template<typename... Args>
            #if SR_ENABLE_LOGGING
                constexpr static void LogCoreMessageIf(const MessageSeverity messageSeverity, const bool expression, fmt::format_string<Args...> fmt, Args &&...args)
                {
                    if (expression) LogCoreMessage(messageSeverity, fmt, std::forward<Args>(args)...);
                }
            #else
                constexpr static void LogCoreMessageIf(const MessageSeverity messageSeverity, const bool expression, Args &&...args) { }
            #endif
        #endif

    private:
        friend class Application;
        static void Initialize(const std::string &applicationName);

        #if SR_ENABLE_LOGGING
            inline static std::shared_ptr<spdlog::logger> engineLogger = nullptr;
            inline static std::shared_ptr<spdlog::logger> applicationLogger = nullptr;
        #endif

    };

}

#if SR_ENABLE_LOGGING
    #if defined(SR_LIBRARY_IMPLEMENTATION)
        #define SR_INFO(...) ::Sierra::Logger::LogCoreMessage(::Sierra::MessageSeverity::Info, __VA_ARGS__)
        #define SR_WARNING(...) ::Sierra::Logger::LogCoreMessage(::Sierra::MessageSeverity::Warning, __VA_ARGS__)
        #define SR_ERROR(...) ::Sierra::Logger::LogCoreMessage(::Sierra::MessageSeverity::Error, __VA_ARGS__)

        #define SR_INFO_IF(EXPRESSION, ...) ::Sierra::Logger::LogCoreMessageIf(::Sierra::MessageSeverity::Info, EXPRESSION, __VA_ARGS__)
        #define SR_WARNING_IF(EXPRESSION, ...) ::Sierra::Logger::LogCoreMessageIf(::Sierra::MessageSeverity::Warning, EXPRESSION, __VA_ARGS__)
        #define SR_ERROR_IF(EXPRESSION, ...) ::Sierra::Logger::LogCoreMessageIf(::Sierra::MessageSeverity::Error, EXPRESSION, __VA_ARGS__)
    #endif

    #define APP_INFO(...) ::Sierra::Logger::LogMessage(::Sierra::MessageSeverity::Info, __VA_ARGS__)
    #define APP_WARNING(...) ::Sierra::Logger::LogMessage(::Sierra::MessageSeverity::Warning, __VA_ARGS__)
    #define APP_ERROR(...) ::Sierra::Logger::LogMessage(::Sierra::MessageSeverity::Error, __VA_ARGS__)

    #define APP_INFO_IF(EXPRESSION, ...) ::Sierra::Logger::LogMessageIf(::Sierra::MessageSeverity::Info, EXPRESSION, __VA_ARGS__)
    #define APP_WARNING_IF(EXPRESSION, ...) ::Sierra::Logger::LogMessageIf(::Sierra::MessageSeverity::Warning, EXPRESSION, __VA_ARGS__)
    #define APP_ERROR_IF(EXPRESSION, ...) ::Sierra::Logger::LogMessageIf(::Sierra::MessageSeverity::Error, EXPRESSION, __VA_ARGS__)
#else
    #if defined(SR_LIBRARY_IMPLEMENTATION)
        #define SR_INFO(...)
        #define SR_WARNING(...)
        #define SR_ERROR(...)

        #define SR_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
        #define SR_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
        #define SR_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #endif

    #define APP_INFO(...)
    #define APP_WARNING(...)
    #define APP_ERROR(...)

    #define APP_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #define APP_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #define APP_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
#endif