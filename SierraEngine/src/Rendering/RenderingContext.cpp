//
// Created by Nikolay Kanchevski on 27.10.25.
//

#include "RenderingContext.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    RenderingContext::RenderingContext(const RenderingContextCreateInfo& createInfo)
    {
        device = createInfo.renderingInstance.CreateDevice({ .name = "Rendering Context Device" });
        resourceTable = device->CreateResourceTable({ .name = "Rendering Context Resource Table" });
        destructionScheduler = device->CreateDestructionScheduler({ .name = "Rendering Context Destruction Scheduler" });
    }

    /* --- POLLING METHODS --- */

    void RenderingContext::Bind(Sierra::CommandBuffer& commandBuffer) const
    {
        destructionScheduler->Flush();
        commandBuffer.BindResourceTable(*resourceTable);
    }

}