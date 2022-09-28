//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include <iostream>
#include "VulkanDebugger.h"

#define DEFAULT "\e[0;32m"
#define BLUE "\e[0;34m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define RED "\e[0;31m"

namespace Sierra::Core::Rendering::Vulkan
{

    #if DEBUG
        static const bool DEBUG_MODE = true;
    #else
        static const bool DEBUG_MODE = false;
    #endif

    VulkanDebugger::MessageType VulkanDebugger::lastMessageType = Info;

    void VulkanDebugger::DisplayInfo(const std::string& message)
    {
        if (lastMessageType != Info) std::cout << "\n";

        printf(BLUE);
        std::cout << "[i] " << message << ".\n";
        printf(DEFAULT);

        lastMessageType = Info;
    }

    void VulkanDebugger::DisplaySuccess(const std::string& message)
    {
        if (lastMessageType != Success) std::cout << "\n";

        printf(GREEN);
        std::cout << "[+] " << message << "!\n";
        printf(DEFAULT);

        lastMessageType = Success;
    }

    void VulkanDebugger::ThrowWarning(const std::string& message)
    {
        if (lastMessageType != Warning) std::cout << "\n";

        printf(YELLOW);
        std::cout << "[!] " << message << "!\n";
        printf(DEFAULT);

        lastMessageType = Warning;
    }

    void VulkanDebugger::ThrowError(const std::string& message)
    {
        if (lastMessageType != Error) std::cout << "\n";

        printf(RED);
        std::cout << "[-] " << message << "!\n";
        printf(DEFAULT);

        if (DEBUG_MODE) exit(-1);

        lastMessageType = Error;
    }
}