//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

#include "../../../../Engine/Classes/Mesh.h"

using Sierra::Engine::Classes::PushConstant;

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreatePushConstants()
    {
        // Get push constant size
        pushConstantSize = sizeof(PushConstant);

        // Set up vertex shader's push constants
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = pushConstantSize;
    }

}
