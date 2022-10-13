//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateTextureSampler()
    {
        textureSampler = Sampler::Builder()
            .SetMaxAnisotropy(1.0f)
            .ApplyBilinearFiltering(true)
        .Build();
    }

    void VulkanRenderer::CreateNullTextures()
    {

    }

}