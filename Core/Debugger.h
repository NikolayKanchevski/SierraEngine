//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <string>
#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>

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
        static std::string TypeToString() {

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
    #define ASSERT_ERROR_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_ERROR(MESSAGE)

    #define ASSERT_WARNING(MESSAGE) Debugger::ThrowWarning(MESSAGE)
    #define ASSERT_WARNING_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_WARNING(MESSAGE)

    #define ASSERT_SUCCESS(MESSAGE) Debugger::DisplaySuccess(MESSAGE)
    #define ASSERT_SUCCESS_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_SUCCESS(MESSAGE)

    #define ASSERT_INFO(MESSAGE) Debugger::DisplayInfo(MESSAGE)
    #define ASSERT_INFO_IF(EXPRESSION, MESSAGE) if (EXPRESSION) ASSERT_INFO(MESSAGE)

    #define VK_ASSERT(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) ASSERT_ERROR(std::string("Vulkan Error: ") + std::string(#FUNCTION).substr(0, std::string(#FUNCTION).find_first_of("(")) + std::string("() failed: ") + MESSAGE + "! Error code: " + string_VkResult(result))
    #define VK_VALIDATE(FUNCTION, MESSAGE) if (VkResult result = FUNCTION; result != VK_SUCCESS) ASSERT_WARNING(std::string("Vulkan Error: ") + std::string(#FUNCTION).substr(0, std::string(#FUNCTION).find_first_of("(")) + std::string("() failed: ") + MESSAGE + "! Error code: " + string_VkResult(result))
#else
    #define ASSERT_ERROR(MESSAGE)
    #define ASSERT_ERROR_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)

    #define ASSERT_WARNING(MESSAGE)
    #define ASSERT_WARNING_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)

    #define ASSERT_SUCCESS(MESSAGE)
    #define ASSERT_SUCCESS_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)

    #define ASSERT_INFO(MESSAGE)
    #define ASSERT_INFO_IF(EXPRESSION, MESSAGE) static_cast<void>(EXPRESSION)

    #define VK_ASSERT(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
    #define VK_VALIDATE(FUNCTION, MESSAGE) static_cast<void>(FUNCTION)
#endif

