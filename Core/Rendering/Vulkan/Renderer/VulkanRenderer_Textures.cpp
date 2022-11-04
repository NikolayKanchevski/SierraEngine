//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateNullTextures()
    {
        // Create default diffuse texture
        nullDiffuseTexture = Texture::Create({
           .filePath = "Textures/Null/DiffuseNull.jpg",
           .textureType = TEXTURE_TYPE_DIFFUSE,
           .samplerCreateInfo {
               .applyBilinearFiltering = false
           }
        }, true);

        // Create default specular texture
        nullSpecularTexture = Texture::Create({
           .filePath = "Textures/texture1.jpg",
           .textureType = TEXTURE_TYPE_SPECULAR,
        }, true);

        if (DescriptorInfo::DESCRIPTOR_INDEXING_SUPPORTED)
        {
            globalBindlessDescriptorSet = BindlessDescriptorSet::Build({ 3 }, DESCRIPTOR_TYPE_TEXTURE_TRANSFER, descriptorPool);
            globalBindlessDescriptorSet->WriteTexture(3, nullDiffuseTexture, 0);
            globalBindlessDescriptorSet->WriteTexture(3, nullSpecularTexture, 1);
            globalBindlessDescriptorSet->Allocate();
        }
    }

}