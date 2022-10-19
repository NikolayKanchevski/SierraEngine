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
