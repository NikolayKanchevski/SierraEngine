//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#define PROFILE_FUNCTIONS_IN_RELEASE 0

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
        static void DisplayInfo(const std::string&);
        static void DisplaySuccess(const std::string&);
        static void ThrowWarning(const std::string&);
        static void ThrowError(const std::string&);

        static bool CheckResults(const VkResult result, const std::string&);

        template <typename T>
        static inline std::string TypeToString()
        {
            auto unformatted = Demangle(typeid(T).name());
            return unformatted.substr(unformatted.find_last_of(':') + 1);
        }

        static VKAPI_ATTR VkBool32 VKAPI_CALL ValidationCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
        );

    private:
        static std::string Demangle(const char* name);

    };
}

#if DEBUG
    #define ASSERT_ERROR(MESSAGE) Debugger::ThrowError(MESSAGE)
    #define ASSERT_ERROR_FORMATTED(MESSAGE, VALUES...) ASSERT_ERROR(fmt::format(MESSAGE, VALUES))
    #define ASSERT_ERROR_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_ERROR(MESSAGE)
    #define ASSERT_ERROR_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) if (EXPRESSION) ASSERT_ERROR_FORMATTED(MESSAGE, VALUES)

    #define ASSERT_WARNING(MESSAGE) Debugger::ThrowWarning(MESSAGE)
    #define ASSERT_WARNING_FORMATTED(MESSAGE, VALUES...) ASSERT_WARNING(fmt::format(MESSAGE, VALUES))
    #define ASSERT_WARNING_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_WARNING(MESSAGE)
    #define ASSERT_WARNING_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) if (EXPRESSION) ASSERT_WARNING_FORMATTED(MESSAGE, VALUES)

    #define ASSERT_SUCCESS(MESSAGE) Debugger::DisplaySuccess(MESSAGE)
    #define ASSERT_SUCCESS_FORMATTED(MESSAGE, VALUES...) ASSERT_SUCCESS(fmt::format(MESSAGE, VALUES))
    #define ASSERT_SUCCESS_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_SUCCESS(MESSAGE)
    #define ASSERT_SUCCESS_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) if (EXPRESSION) ASSERT_SUCCESS_FORMATTED(MESSAGE, VALUES)

    #define VK_ASSERT(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) ASSERT_ERROR_FORMATTED("Vulkan Error: {0}() failed: {1}! Error code: {2}", std::string(#FUNCTION).substr(0, std::string(#FUNCTION).find_first_of("(")), MESSAGE, string_VkResult(result))
    #define VK_VALIDATE(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) ASSERT_WARNING_FORMATTED("Vulkan Error: {0}() failed: {1}! Error code: {2}", std::string(#FUNCTION).substr(0, std::string(#FUNCTION).find_first_of("(")), MESSAGE, string_VkResult(result))
#else
    #define ASSERT_ERROR(MESSAGE)
    #define ASSERT_ERROR_FORMATTED(MESSAGE, VALUES...)
    #define ASSERT_ERROR_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_ERROR_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) static_cast<void>(EXPRESSION)

    #define ASSERT_WARNING(MESSAGE)
    #define ASSERT_WARNING_FORMATTED(MESSAGE, VALUES...)
    #define ASSERT_WARNING_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_WARNING_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) static_cast<void>(EXPRESSION)

    #define ASSERT_SUCCESS(MESSAGE)
    #define ASSERT_SUCCESS_FORMATTED(MESSAGE, VALUES...)
    #define ASSERT_SUCCESS_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_SUCCESS_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) static_cast<void>(EXPRESSION)

    #define ASSERT_INFO(MESSAGE)
    #define ASSERT_INFO_FORMATTED(MESSAGE, VALUES...)
    #define ASSERT_INFO_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)
    #define ASSERT_INFO_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) static_cast<void>(EXPRESSION)

    #define VK_ASSERT(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
    #define VK_VALIDATE(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
#endif

#if DEBUG || PROFILE_FUNCTIONS_IN_RELEASE
    #define ASSERT_INFO(MESSAGE) Debugger::DisplayInfo(MESSAGE)
    #define ASSERT_INFO_FORMATTED(MESSAGE, VALUES...) ASSERT_INFO(fmt::format(MESSAGE, VALUES))
    #define ASSERT_INFO_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_INFO(MESSAGE)
    #define ASSERT_INFO_FORMATTED_IF(EXPRESSION, MESSAGE, VALUES...) if (EXPRESSION) ASSERT_INFO_FORMATTED(MESSAGE, VALUES)

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

    #define PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::CleanupOutputString(name, "__cdecl ");\
                                                   ::InstrumentationTimer timer##line(fixedName##line.Data)
    #define PROFILE_SCOPE_LINE(name, line) PROFILE_SCOPE_LINE2(name, line)
    #define PROFILE_SCOPE(name) PROFILE_SCOPE_LINE(name, __LINE__)
    #define PROFILE_FUNCTION() PROFILE_SCOPE(FUNC_SIG)
#else
    #define PROFILE_FUNCTION()
#endif

template <size_t N>
struct ChangeResult
{
    char Data[N];
};

template <size_t N, size_t K>
constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K])
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


using namespace Sierra::Core;

class InstrumentationTimer
{

public:
    InstrumentationTimer(const char* name)
            : name(name), stopped(false)
    {
        startTimepoint = std::chrono::steady_clock::now();
    }

    ~InstrumentationTimer()
    {
        if (!stopped) Stop();
    }

    void Stop()
    {
        auto endTimePoint = std::chrono::steady_clock::now();
        auto highResStart = std::chrono::duration_cast<std::chrono::milliseconds>(startTimepoint.time_since_epoch()).count();
        auto highResEnd = std::chrono::duration_cast<std::chrono::milliseconds>(endTimePoint.time_since_epoch()).count();
        auto elapsedTime = highResEnd - highResStart;

        std::string nameString = std::string(name);

        uint i = nameString.size();
        while (true)
        {
            if (nameString[i] == '(')
            {
                uint j = i;
                uint columnCounter = 0;

                while (columnCounter < 3)
                {
                    j--;

                    if (j == -1)
                    {
                        break;
                    }
                    else if (nameString[j] == ':')
                    {
                        columnCounter++;
                    }
                }

                j++;

                nameString = nameString.substr(j, i - j + 1) + ')';
                break;
            }

            i--;
        }

        ASSERT_INFO("Method " + nameString + " took " + std::to_string(elapsedTime) + "ms");

        stopped = true;
    }

private:
    const char* name;
    std::chrono::time_point<std::chrono::steady_clock> startTimepoint;
    bool stopped;

};