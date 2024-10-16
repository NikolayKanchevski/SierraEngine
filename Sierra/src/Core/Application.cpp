//
// Created by Nikolay Kanchevski on 21.08.23.
//

#include "Application.h"

#if SR_PLATFORM_WINDOWS
    #include "../Platform/Windows/WindowsContext.h"
#elif SR_PLATFORM_LINUX
    #include "../Platform/Linux/LinuxContext.h"
#elif SR_PLATFORM_macOS
    #include "../Platform/macOS/macOSContext.h"
#elif SR_PLATFORM_iOS
    #include "../Platform/iOS/iOSContext.h"
#elif SR_PLATFORM_ANDROID
    #include "../Platform/Android/AndroidContext.h"
#endif

#if SR_VULKAN_SUPPORTED
    #include "../Rendering/Vulkan/VulkanContext.h"
#endif
#if SR_METAL_SUPPORTED
    #include "../Rendering/Metal/MetalContext.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo& createInfo)
        : name(createInfo.name), version(createInfo.version)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create application, as specified name must not be empty"));
        #if SR_ENABLE_LOGGING
            Logger::Initialize(name);
        #endif

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

        // Create rendering context
        {
            const RenderingContextCreateInfo renderingContextCreateInfo
            {
                .applicationName = GetName(),
                .applicationVersion = GetVersion()
            };

            switch (createInfo.settings.renderingBackendType)
            {
                case RenderingBackendType::Vulkan:
                {
                    #if !SR_VULKAN_SUPPORTED
                        SR_THROW(UnsupportedFeatureError(SR_FORMAT("Cannot create application [{0}], as specified backend type [RenderingBackendType::Vulkan] is either unsupported on this system, or the CMake option [SIERRA_BUILD_VULKAN] is not turned on", name)));
                    #else
                        renderingContext = std::make_unique<VulkanContext>(renderingContextCreateInfo);
                    #endif
                    break;
                }
                case RenderingBackendType::DirectX:
                {
                    SR_THROW(UnsupportedFeatureError("Cannot create application, as specified backend type [RenderingBackendType::WebGPU] has not been implemented yet"));
                    break;
                }
                case RenderingBackendType::Metal:
                {
                    #if !SR_METAL_SUPPORTED
                        SR_THROW(UnsupportedFeatureError(SR_FORMAT("Cannot create application [{0}], as specified backend type [RenderingBackendType::Metal] is either unsupported on this system, or the CMake option [SIERRA_BUILD_METAL] is not turned on", name)));
                    #else
                        renderingContext = std::make_unique<MetalContext>(renderingContextCreateInfo);
                    #endif
                    break;
                }
                case RenderingBackendType::OpenGL:
                {
                    SR_THROW(UnsupportedFeatureError("Cannot create application, as specified backend type [RenderingBackendType::OpenGL] has not been implemented yet"));
                    break;
                }
                case RenderingBackendType::WebGPU:
                {
                    SR_THROW(UnsupportedFeatureError("Cannot create application, as specified backend type [RenderingBackendType::WebGPU] has not been implemented yet"));
                    break;
                }
            }
        }
    }

    /* --- POLLING METHODS --- */

    bool Application::Update()
    {
        platformContext->Update();
        return false;
    }

    /* --- PROTECTED METHODS --- */

    std::filesystem::path Application::GetApplicationTemporaryDirectoryPath() const noexcept
    {
        return GetFileManager().GetTemporaryDirectoryPath() / name;
    }

    std::filesystem::path Application::GetApplicationCachesDirectoryPath() const noexcept
    {
        return GetFileManager().GetCachesDirectoryPath() / name;
    }

}
