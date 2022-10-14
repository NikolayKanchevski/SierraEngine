//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateNullTextures()
    {
        // Create default diffuse texture
        nullDiffuseTexture = Texture::Builder(descriptorPool)
            .SetTextureType(TEXTURE_TYPE_DIFFUSE)
            .ApplyBilinearFiltering(false)
        .Build("Textures/Null/DiffuseNull.jpg");

        // Create default specular texture
        nullSpecularTexture = Texture::Builder(descriptorPool)
            .SetTextureType(TEXTURE_TYPE_SPECULAR)
        .Build("Textures/texture1.jpg");

        // Write textures to descriptor set
        nullTexturesDescriptorSet = DescriptorSet::Build(descriptorPool);
        nullTexturesDescriptorSet->WriteTexture(1, nullDiffuseTexture);
        nullTexturesDescriptorSet->WriteTexture(2, nullSpecularTexture);
        nullTexturesDescriptorSet->Allocate();
    }

}