//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <string>
#include <vulkan/vulkan.h>

#if _WIN32
    #include <windows.h>
#endif

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanDebugger
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
    private:
        enum MessageType { Info, Success, Warning, Error };
        static MessageType lastMessageType;
    };
}
