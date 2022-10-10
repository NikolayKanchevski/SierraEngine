//
// Created by Nikolay Kanchevski on 9.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    struct alignas(16) PushMaterial
    {
        glm::vec3 diffuse;
        float shininess;

        glm::vec3 specular;
        glm::vec3 ambient;
    };

    struct PushConstant
    {
        /* VERTEX DATA */
        glm::mat4x4 modelMatrix;

        /* FRAGMENT DATA */
        PushMaterial material;
    };

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
