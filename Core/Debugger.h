//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#define ENABLE_DEBUGGING_IN_RELEASE 1
#define PROFILE_FUNCTIONS_IN_RELEASE 1

#if _WIN32
    #include <windows.h>
#endif

namespace Sierra::Core
{
    class Debugger
    {

#if _WIN32
    static HANDLE hConsole;
#endif

    public:
        static void DisplayInfo(const String &message);
        static void DisplaySuccess(const String &message);
        static void ThrowWarning(const String &message);
        static void ThrowError(const String &message);

        template <typename T>
        static inline std::string TypeToString()
        {
            auto unformatted = Demangle(typeid(T).name());
            return unformatted.substr(unformatted.find_last_of(':') + 1);
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

        template <size_t N>
        struct ChangeResult
        {
            char Data[N];
        };

        template <size_t N, size_t K>
        static constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
        {
            ChangeResult<N> result = {};

            size_t srcIndex = 0;
            size_t dstIndex = 0;

            while (srcIndex < N)
            {
                size_t matchIndex = 0;

                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex]) matchIndex++;
                if (matchIndex == K - 1) srcIndex += matchIndex;

                result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
                srcIndex++;
            }

            return result;
        }


        class FunctionProfiler
        {
        public:
            explicit FunctionProfiler(const char* name);

            void Stop();
            ~FunctionProfiler();

        private:
            const char* name;
            std::chrono::time_point<std::chrono::steady_clock> startTimepoint;
            bool stopped = false;
        };

    private:
        static std::string Demangle(const char* name);

    };
}

#if DEBUG || ENABLE_DEBUGGING_IN_RELEASE
    #define ASSERT_ERROR(MESSAGE) Sierra::Core::Debugger::ThrowError(FORMAT_STRING("File {0}:{1} encountered an error on line: {2}", THIS_FILE, THIS_LINE, MESSAGE))
    #define ASSERT_ERROR_FORMATTED(MESSAGE, ...) ASSERT_ERROR(FORMAT_STRING(MESSAGE, ##__VA_ARGS__))
    #define ASSERT_ERROR_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_ERROR(MESSAGE)
    #define ASSERT_ERROR_FORMATTED_IF(EXPRESSION, MESSAGE, ...) if (EXPRESSION) ASSERT_ERROR_FORMATTED(MESSAGE, ##__VA_ARGS__)

    #define ASSERT_WARNING(MESSAGE) Sierra::Core::Debugger::ThrowWarning(MESSAGE)
    #define ASSERT_WARNING_FORMATTED(MESSAGE, ...) ASSERT_WARNING(FORMAT_STRING(MESSAGE, ##__VA_ARGS__))
    #define ASSERT_WARNING_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_WARNING(MESSAGE)
    #define ASSERT_WARNING_FORMATTED_IF(EXPRESSION, MESSAGE, ...) if (EXPRESSION) ASSERT_WARNING_FORMATTED(MESSAGE, ##__VA_ARGS__)

    #define ASSERT_SUCCESS(MESSAGE) Sierra::Core::Debugger::DisplaySuccess(MESSAGE)
    #define ASSERT_SUCCESS_FORMATTED(MESSAGE, ...) ASSERT_SUCCESS(FORMAT_STRING(MESSAGE, ##__VA_ARGS__))
    #define ASSERT_SUCCESS_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_SUCCESS(MESSAGE)
    #define ASSERT_SUCCESS_FORMATTED_IF(EXPRESSION, MESSAGE, ...) if (EXPRESSION) ASSERT_SUCCESS_FORMATTED(MESSAGE, ##__VA_ARGS__)

    #define VK_ASSERT(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) ASSERT_ERROR_FORMATTED("Vulkan Error: {0}() failed: {1}! Error code: {2}", std::string(#FUNCTION).substr(0, std::string(#FUNCTION).find_first_of("(")), MESSAGE, VK_TO_STRING(result, Result))
    #define VK_VALIDATE(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) ASSERT_WARNING_FORMATTED("Vulkan Error: {0}() failed: {1}! Error code: {2}", std::string(#FUNCTION).substr(0, std::string(#FUNCTION).find_first_of("(")), MESSAGE, VK_TO_STRING(result, Result))

    #define NULL_ASSERT(VALUE) if (VALUE == nullptr) { ASSERT_ERROR(FORMAT_STRING("Value \"{0}\" is null", #VALUE)); }
    #define STATIC_ASSERT_IF(CONDITION, MESSAGE) static_assert(!CONDITION, MESSAGE)
#else
    #define ASSERT_ERROR(MESSAGE)
    #define ASSERT_ERROR_FORMATTED(MESSAGE, ...)
    #define ASSERT_ERROR_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_ERROR_FORMATTED_IF(EXPRESSION, MESSAGE, ...) static_cast<void>(EXPRESSION)

    #define ASSERT_WARNING(MESSAGE)
    #define ASSERT_WARNING_FORMATTED(MESSAGE, ...)
    #define ASSERT_WARNING_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_WARNING_FORMATTED_IF(EXPRESSION, MESSAGE, ...) static_cast<void>(EXPRESSION)

    #define ASSERT_SUCCESS(MESSAGE)
    #define ASSERT_SUCCESS_FORMATTED(MESSAGE, ...)
    #define ASSERT_SUCCESS_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_SUCCESS_FORMATTED_IF(EXPRESSION, MESSAGE, ...) static_cast<void>(EXPRESSION)

    #if !DEBUG && !PROFILE_FUNCTIONS_IN_RELEASE
        #define ASSERT_INFO(MESSAGE)
        #define ASSERT_INFO_FORMATTED(MESSAGE, ...)
        #define ASSERT_INFO_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
        #define ASSERT_INFO_FORMATTED_IF(EXPRESSION, MESSAGE, ...) static_cast<void>(EXPRESSION)
    #endif

    #define VK_ASSERT(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
    #define VK_VALIDATE(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
#endif

#if DEBUG || PROFILE_FUNCTIONS_IN_RELEASE
    #define ASSERT_INFO(MESSAGE) Sierra::Core::Debugger::DisplayInfo(MESSAGE)
    #define ASSERT_INFO_FORMATTED(MESSAGE, ...) ASSERT_INFO(FORMAT_STRING(MESSAGE, ##__VA_ARGS__))
    #define ASSERT_INFO_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_INFO(MESSAGE)
    #define ASSERT_INFO_FORMATTED_IF(EXPRESSION, MESSAGE, ...) if (EXPRESSION) ASSERT_INFO_FORMATTED(MESSAGE, ##__VA_ARGS__)

    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
    #define FUNC_SIG __PRETTY_FUNCTION__
        #elif (defined(__FUNCSIG__) || (_MSC_VER))
            #define FUNC_SIG __FUNCSIG__
        #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
            #define FUNC_SIG __FUNCTION__
        #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
            #define FUNC_SIG __FUNC__
        #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
            #define FUNC_SIG __func__
        #elif defined(__cplusplus) && (__cplusplus >= 201103)
            #define FUNC_SIG __func__
        #else
            #define FUNC_SIG "FUNC_SIG unknown!"
    #endif

    #define PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = Sierra::Core::Debugger::CleanupOutputString(name, "__cdecl ");\
                                                   Sierra::Core::Debugger::FunctionProfiler timer##line(fixedName##line.Data)
    #define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE2(name, line)
    #define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(FUNC_SIG)
#else
    #define PROFILE_FUNCTION()
#endif