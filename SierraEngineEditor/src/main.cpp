//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "Core/EditorApplication.h"

Sierra::Application* Sierra::CreateApplication(const int argc, const char* argv[])
{
    return new SierraEngine::EditorApplication({ .name = "Sierra Engine", .version = Sierra::Version({ 1, 0, 0 }) });
}