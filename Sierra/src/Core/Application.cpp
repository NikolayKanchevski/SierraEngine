//
// Created by Nikolay Kanchevski on 21.08.23.
//

#include "Application.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsContext.h"
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/LinuxContext.h"
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/macOSContext.h"
    #include "Platform/Apple/FoundationFileManager.h"
    #include "Platform/macOS/macOSWindowManager.h"
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSContext.h"
    #include "Platform/Apple/FoundationFileManager.h"
    #include "Platform/iOS/iOSWindowManager.h"
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/AndroidContext.h"
#endif

#if defined(SR_VULKAN_SUPPORTED)
    #include "../Rendering/Platform/Vulkan/VulkanContext.h"
#endif
#if defined(SR_METAL_SUPPORTED)
    #include "../Rendering/Platform/Metal/MetalContext.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo& createInfo)
        : name(createInfo.name), version(createInfo.version)
    {
        #if SR_ENABLE_LOGGING
            Logger::Initialize(name);
        #endif
        SR_ERROR_IF(createInfo.name.empty(), "Application title must not be empty!");

        // Create platform context
        {
            #if SR_PLATFORM_WINDOWS
                platformContext = std::make_unique<WindowsContext>();
            #elif SR_PLATFORM_LINUX
                platformContext = std::make_unique<LinuxContext>();
            #elif SR_PLATFORM_macOS
                platformContext = std::make_unique<macOSContext>();
            #elif SR_PLATFORM_iOS
                platformContext = std::make_unique<iOSContext>();
            #elif SR_PLATFORM_ANDROID
                platformContext = std::make_unique<AndroidContext>();
            #endif
        }

        // Create file manager
        {
            #if SR_PLATFORM_WINDOWS

            #elif SR_PLATFORM_LINUX
                platformContext = std::make_unique<LinuxContext>(platformContextCreateInfo);
            #elif SR_PLATFORM_APPLE
                fileManager = std::make_unique<FoundationFileManager>();
            #elif SR_PLATFORM_ANDROID

            #endif
        }

        // Create window manager
        {
            const WindowManagerCreateInfo windowManagerCreateInfo = { .platformContext = *platformContext };
            #if SR_PLATFORM_WINDOWS

            #elif SR_PLATFORM_LINUX

            #elif SR_PLATFORM_macOS
                windowManager = std::make_unique<macOSWindowManager>(windowManagerCreateInfo);
            #elif SR_PLATFORM_iOS
                windowManager = std::make_unique<iOSWindowManager>(windowManagerCreateInfo);
            #elif SR_PLATFORM_ANDROID

            #endif
        }

        // Create rendering context
        {
            switch (createInfo.settings.graphicsAPI)
            {
                case GraphicsAPI::Vulkan:
                {
                    #if !defined(SR_VULKAN_SUPPORTED)
                        SR_ERROR("Cannot create rendering context [{0}] using the Vulkan API, as it is unsupported on the system, or the CMake option [\"SIERRA_BUILD_VULKAN\"] hast not been turned on!", createInfo.name);
                    #else
                        renderingContext = std::make_unique<VulkanContext>(RenderingContextCreateInfo{});
                    #endif
                    break;
                }
                case GraphicsAPI::DirectX:
                {
                    SR_ERROR("Cannot create rendering context using DirectX, as it has not been implemented yet!");
                    break;
                }
                case GraphicsAPI::Metal:
                {
                    #if !defined(SR_METAL_SUPPORTED)
                        SR_ERROR("Cannot create rendering context using the Metal API, as it is unsupported on the system, or the CMake option [\"SIERRA_BUILD_METAL\"] hast not been turned on!");
                    #else
                        renderingContext = std::make_unique<MetalContext>();
                    #endif
                    break;
                }
                case GraphicsAPI::OpenGL:
                {
                    SR_ERROR("Cannot create rendering context [{0}] using OpenGL, as it has not been implemented yet!", createInfo.name);
                    break;
                }
                case GraphicsAPI::Undefined:
                {
                    SR_ERROR("Cannot create rendering context [{0}] using an undefined graphics API!", createInfo.name);
                    break;
                }
            }
        }
    }

    /* --- PROTECTED METHODS --- */

    std::filesystem::path Application::GetApplicationTemporaryDirectoryPath() const
    {
        return fileManager->GetTemporaryDirectoryPath() / name;
    }

    std::filesystem::path Application::GetApplicationCachesDirectoryPath() const
    {
        return fileManager->GetCachesDirectoryPath() / name;
    }

}
