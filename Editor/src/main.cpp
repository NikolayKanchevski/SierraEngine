//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "Core/EditorApplication.h"
using namespace SierraEngine;

int main()
{
    const ApplicationCreateInfo createInfo
    {
        .name = "Sierra Engine",
        .version = Sierra::Version({ 1, 0, 0 })
    };

    const std::unique_ptr<EditorApplication> application = std::make_unique<EditorApplication>(createInfo);
    application->Run();

    return 0;
}