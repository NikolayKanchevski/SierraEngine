//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include "CommandBuffer.h"

namespace Sierra
{

    class SIERRA_API CommandTask
    {
    public:
        /* --- CONSTRUCTORS --- */
        CommandTask() = default;

        /* --- OPERATORS --- */
        CommandTask(const CommandTask&) = delete;
        CommandTask &operator=(const CommandTask&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~CommandTask() = default;

    };

}