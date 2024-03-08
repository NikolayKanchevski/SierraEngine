//
// Created by Nikolay Kanchevski on 18.02.24.
//

#include "Core/Application.h"

int main()
{
    using namespace SierraEngine;
    std::unique_ptr<Application> application = std::make_unique<Application>(Sierra::ApplicationCreateInfo {
        .name = "Sierra Engine",
        .version = Sierra::Version({ 1, 0, 0 }),
//        .settings={.graphicsAPI=Sierra::GraphicsAPI::Vulkan}
    });

    application->Run();
}