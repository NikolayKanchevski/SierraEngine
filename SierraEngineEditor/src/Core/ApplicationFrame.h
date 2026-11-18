//
// Created by Nikolay Kanchevski on 16.11.26.
//

#pragma once

#include "../Core/Project.h"

namespace SierraEngine
{

    struct ApplicationFrame
    {
        Sierra::TimeStep timeStep = { };
        Project& project;

        const Sierra::PlatformContext& platformContext;
        const Sierra::InputManager* inputManager = nullptr;
        const Sierra::CursorManager* cursorManager = nullptr;
        const Sierra::TouchManager* touchManager = nullptr;
    };

}