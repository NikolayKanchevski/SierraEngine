//
// Created by Nikolay Kanchevski on 21.08.23.
//

#include "Application.h"

#include "Sierra/Rendering/Platform/Vulkan/VulkanContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo &createInfo)
        : name(createInfo.name), version(createInfo.version)
    {
        SR_ERROR_IF(createInfo.name.empty(), "Application title must not be empty!");

        // Logger is initialized first, prior to even creating the application
        Logger::Initialize(name);

        // Create application resources
        platformInstance = PlatformInstance::Create({ });
        renderingContext = RenderingContext::Create({ .graphicsAPI = createInfo.graphicsAPI });

        // Initialize engine resources
        InitializeEngine();
    }

    void Application::Run()
    {
        OnStart();
        while (!OnUpdate())
        {
            continue;
        }
    }

    /* --- PRIVATE METHODS --- */

    void Application::InitializeEngine()
    {

    }

    /* --- DESTRUCTOR --- */

    Application::~Application()
    {

    }

}
