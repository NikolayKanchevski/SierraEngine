//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <string>
#include <vulkan/vulkan.h>

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

#define THROW_ERROR_IF(__EXPRESSION__, __MESSAGE__) if (__EXPRESSION__) Debugger::ThrowError(__MESSAGE__)
#define THROW_ERROR(__MESSAGE__) Debugger::ThrowError(__MESSAGE__)
#define VK_DISPLAY(__EXPRESSION__, __MESSAGE__) if (__EXPRESSION__ != VK_SUCCESS) THROW_ERROR(__MESSAGE__)

#if DEBUG
    #define THROW_WARNING_IF(__EXPRESSION__, __MESSAGE__) if (__EXPRESSION__) Debugger::ThrowWarning(__MESSAGE__)
    #define THROW_WARNING(__MESSAGE__) Debugger::ThrowWarning(__MESSAGE__)

    #define DISPLAY_SUCCESS_IF(__EXPRESSION__, __MESSAGE__) if (__EXPRESSION__) Debugger::DisplaySuccess(__MESSAGE__)
    #define DISPLAY_SUCCESS(__MESSAGE__) Debugger::DisplaySuccess(__MESSAGE__)

    #define DISPLAY_INFO_IF(__EXPRESSION__, __MESSAGE__) if (__EXPRESSION__) Debugger::DisplayInfo(__MESSAGE__)
    #define DISPLAY_INFO(__MESSAGE__) Debugger::DisplayInfo(__MESSAGE__)
#else
    #define THROW_WARNING_IF(__EXPRESSION__, __MESSAGE__)
    #define THROW_WARNING(__MESSAGE__)

    #define DISPLAY_SUCCESS_IF(__EXPRESSION__, __MESSAGE__)
    #define DISPLAY_SUCCESS(__MESSAGE__)

    #define DISPLAY_INFO_IF(__EXPRESSION__, __MESSAGE__)
    #define DISPLAY_INFO(__MESSAGE__)
#endif
