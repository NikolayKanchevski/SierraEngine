//
// Created by Nikolay Kanchevski on 14.06.24.
//

#include "Core/RuntimeApplication.h"

Sierra::Application* Sierra::CreateApplication(const int argc, char* argv[])
{
    return new SierraEngine::RuntimeApplication({ .name = "Sierra Engine", .version = Sierra::Version({ 1, 0, 0 }) });
}