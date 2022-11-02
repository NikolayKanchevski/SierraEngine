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
//           .filePath = "Textures/Null/SpecularNull.jpg",
           .filePath = "Textures/texture1.jpg",
           .textureType = TEXTURE_TYPE_SPECULAR,
        }, true);

        globalDescriptorSet = BindlessDescriptorSet::Build(3, DESCRIPTOR_TYPE_TEXTURE_TRANSFER, descriptorPool);
        globalDescriptorSet->WriteTexture(nullDiffuseTexture, 0);
        globalDescriptorSet->WriteTexture(nullSpecularTexture, 1);
        globalDescriptorSet->Allocate();
    }

}