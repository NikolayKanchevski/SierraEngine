//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "Debugger.h"

#if __APPLE__
    #include <cxxabi.h>
#endif

#if _WIN32
    #define DEFAULT() (SetConsoleTextAttribute(hConsole, 7))
    #define BLUE() (SetConsoleTextAttribute(hConsole, 1))
    #define GREEN() (SetConsoleTextAttribute(hConsole, 2))
    #define YELLOW() (SetConsoleTextAttribute(hConsole, 14))
    #define RED() (SetConsoleTextAttribute(hConsole, 4))
#else
    #define DEFAULT() (printf("\e[0;39m"))
    #define BLUE() (printf("\e[0;34m"))
    #define GREEN() (printf("\e[0;32m"))
    #define YELLOW() (printf("\e[0;33m"))
    #define RED() (printf("\e[0;31m"))
#endif

namespace Sierra::Core
{

#if DEBUG
    static const bool DEBUG_MODE = true;
#else
    static const bool DEBUG_MODE = false;
#endif

#if _WIN32
    HANDLE Debugger::hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

    void Debugger::DisplayInfo(const String& message)
    {
        BLUE();
        std::cout << "[i] " << message << ".\n";
        DEFAULT();
    }

    void Debugger::DisplaySuccess(const String& message)
    {
        GREEN();
        std::cout << "[+] " << message << ".\n";
        DEFAULT();
    }

    void Debugger::ThrowWarning(const String& message)
    {
        YELLOW();
        std::cout << "[!] " << message << "!\n";
        DEFAULT();
    }

    void Debugger::ThrowError(const String& message)
    {
        RED();
        std::cout << "[-] " << message << "!\n";
        if (DEBUG_MODE) throw std::runtime_error("Program execution failed miserably!");
        DEFAULT();
    }

    bool Debugger::CheckResults(const VkResult result, const String& errorMessage)
    {
        bool success = result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR;
        if (!success)
        {
            ThrowError(errorMessage + ". Error code: " + string_VkResult(result));
        }

        return success;
    }

    VkBool32 Debugger::ValidationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData)
    {
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                ThrowWarning(String(pCallbackData->pMessage).substr(0, strlen(pCallbackData->pMessage) - 1));
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                ThrowError(String(pCallbackData->pMessage).substr(0, strlen(pCallbackData->pMessage) - 1));
                break;
            default:
                break;
        }

        return VK_FALSE;
    }

    #ifdef __GNUG__
        String Debugger::Demangle(const char* name) {

            int status = -4; // Some arbitrary value to eliminate the compiler warning

            std::unique_ptr<char, void(*)(void*)> res
            {
                abi::__cxa_demangle(name, NULL, NULL, &status),
                std::free
            };

            return (status == 0) ? res.get() : name;
        }
    #else
        // Do nothing if not g++
        String Debugger::Demangle(const char* tag) {
            return tag;
        }
    #endif
}
