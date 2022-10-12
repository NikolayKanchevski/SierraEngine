//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include <iostream>
#include "VulkanDebugger.h"
#include <vulkan/vk_enum_string_helper.h>

#if _WIN32
    #define DEFAULT() (SetConsoleTextAttribute(hConsole, 7))
    #define BLUE() (SetConsoleTextAttribute(hConsole, 1))
    #define GREEN() (SetConsoleTextAttribute(hConsole, 2))
    #define YELLOW() (SetConsoleTextAttribute(hConsole, 14))
    #define RED() (SetConsoleTextAttribute(hConsole, 4))
#else
    #define DEFAULT() (printf("\e[0;32m"))
    #define BLUE() (printf("\e[0;34m"))
    #define GREEN() (printf("\e[0;32m"))
    #define YELLOW() (printf("\e[0;33m"))
    #define RED() (printf("\e[0;31m"))
#endif

namespace Sierra::Core::Rendering::Vulkan
{

#if DEBUG
    static const bool DEBUG_MODE = true;
#else
    static const bool DEBUG_MODE = false;
#endif

#if _WIN32
    HANDLE VulkanDebugger::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    void VulkanDebugger::DisplayInfo(const std::string& message)
    {
        BLUE();
        std::cout << "[i] " << message << ".\n";
        DEFAULT();
    }

    void VulkanDebugger::DisplaySuccess(const std::string& message)
    {
        GREEN();
        std::cout << "[+] " << message << ".\n";
        DEFAULT();
    }

    void VulkanDebugger::ThrowWarning(const std::string& message)
    {
        YELLOW();
        std::cout << "[!] " << message << "!\n";
        DEFAULT();
    }

    void VulkanDebugger::ThrowError(const std::string& message)
    {
        RED();
        if (!DEBUG_MODE) std::cout << "[-] " << message << "!\n";
        else throw std::runtime_error("[-] " + message + "!\n");
        DEFAULT();
    }

    bool VulkanDebugger::CheckResults(const VkResult result, const std::string& errorMessage)
    {
        bool success = result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
        if (!success)
        {
            ThrowError(errorMessage + ". Error code: " + string_VkResult(result));
        }

        return success;
    }

    VkBool32 VulkanDebugger::ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                ThrowWarning(std::string(pCallbackData->pMessage).substr(0, strlen(pCallbackData->pMessage) - 1));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                ThrowError(std::string(pCallbackData->pMessage).substr(0, strlen(pCallbackData->pMessage) - 1));
                break;
            default:
//                std::cout << pCallbackData->pMessage << "\n";
                break;
        }

        return VK_FALSE;
    }
}