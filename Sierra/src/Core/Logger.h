//
// Created by Nikolay Kanchevski on 8.11.2023.
//

#pragma once

#if SR_ENABLE_LOGGING
    #include <spdlog/spdlog.h>
#endif

namespace Sierra
{

    enum class MessageSeverity : uint8
    {
        Info,
        Warning,
        Error
    };

    class SIERRA_API Logger final
    {
    public:
        /* --- POLLING METHODS --- */
        template<typename... Args>
        #if SR_ENABLE_LOGGING
            static void LogMessage(const MessageSeverity messageSeverity, fmt::format_string<Args...> fmt, Args&&... args)
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
                        throw std::runtime_error("Program execution failed miserably!");
                    }
                }
            }
        #else
            constexpr static void LogMessage(const MessageSeverity messageSeverity, Args&&... args) { }
        #endif

        #if defined(SR_LIBRARY_IMPLEMENTATION)
            template<typename... Args>
            #if SR_ENABLE_LOGGING
                constexpr static void LogCoreMessage(const MessageSeverity messageSeverity, fmt::format_string<Args...> fmt, Args&&... args)
                {
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
                            #else
                                break;
                            #endif
                        }
                    }
                }
            #else
                constexpr static void LogCoreMessage(const MessageSeverity messageSeverity, Args&&... args) { }
            #endif
        #endif

    private:
        friend class Application;
        static void Initialize(std::string_view applicationName);

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

        #define SR_INFO_IF(EXPRESSION, ...) if (EXPRESSION) { SR_INFO(__VA_ARGS__); }
        #define SR_WARNING_IF(EXPRESSION, ...) if (EXPRESSION) { SR_WARNING(__VA_ARGS__); }
        #define SR_ERROR_IF(EXPRESSION, ...) if (EXPRESSION) { SR_ERROR(__VA_ARGS__); }
    #endif

    #define APP_INFO(...) ::Sierra::Logger::LogMessage(::Sierra::MessageSeverity::Info, __VA_ARGS__)
    #define APP_WARNING(...) ::Sierra::Logger::LogMessage(::Sierra::MessageSeverity::Warning, __VA_ARGS__)
    #define APP_ERROR(...) ::Sierra::Logger::LogMessage(::Sierra::MessageSeverity::Error, __VA_ARGS__)

    #define APP_INFO_IF(EXPRESSION, ...) if (EXPRESSION) { APP_INFO(__VA_ARGS__); }
    #define APP_WARNING_IF(EXPRESSION, ...) if (EXPRESSION) { APP_WARNING(__VA_ARGS__); }
    #define APP_ERROR_IF(EXPRESSION, ...) if (EXPRESSION) { APP_ERROR(__VA_ARGS__); }
#else
    #if defined(SR_LIBRARY_IMPLEMENTATION)
        #define SR_INFO(...)
        #define SR_WARNING(...)
        #define SR_ERROR(...)

        #define SR_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
        #define SR_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
        #define SR_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #else
        #define APP_INFO(...)
        #define APP_WARNING(...)
        #define APP_ERROR(...)

        #define APP_INFO_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
        #define APP_WARNING_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
        #define APP_ERROR_IF(EXPRESSION, ...) static_cast<void>(EXPRESSION)
    #endif
#endif