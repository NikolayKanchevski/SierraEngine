//
// Created by Nikolay Kanchevski on 16.11.26.
//

#pragma once

namespace SierraEngine
{

    struct SIERRA_ENGINE_API DrawFrame
    {
        const Sierra::Device& device;
        Sierra::CommandBuffer& commandBuffer;

        Sierra::ResourceTable& resourceTable;
        Sierra::DestructionScheduler& destructionScheduler;
    };

}