//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <string>

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanDebugger
    {
    public:
        static void DisplayInfo(const std::string&);
        static void DisplaySuccess(const std::string&);
        static void ThrowWarning(const std::string&);
        static void ThrowError(const std::string&);
    private:
        enum MessageType { Info, Success, Warning, Error };
        static MessageType lastMessageType;
    };
}
