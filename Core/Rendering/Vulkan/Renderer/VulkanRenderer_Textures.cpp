//
// Created by Nikolay Kanchevski on 10.10.22.
//

#include "VulkanRenderer.h"

#include "../VulkanCore.h"
#include "../Abstractions/Texture.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateNullTextures()
    {
        // Create default diffuse texture
        Texture::Create({
           .filePath = "Textures/Null/DiffuseNull.jpg",
           .textureType = TEXTURE_TYPE_DIFFUSE,
           .samplerCreateInfo {
               .applyBilinearFiltering = false
           }
        }, true);

        // Create default specular texture
        Texture::Create({
           .filePath = "Textures/Null/SpecularNull.jpg",
           .textureType = TEXTURE_TYPE_SPECULAR
        }, true);


        // Create default height map texture
        Texture::Create({
            .filePath = "Textures/Null/HeightMapNull.jpg",
            .textureType = TEXTURE_TYPE_HEIGHT_MAP
        }, true);

        // If descriptor indexing supported write default textures to the global descriptor set
        if (VulkanCore::GetDescriptorIndexingSupported())
        {
            globalBindlessDescriptorSet = BindlessDescriptorSet::Build({ BINDLESS_TEXTURE_BINDING }, descriptorPool);
            globalBindlessDescriptorSet->WriteTexture(BINDLESS_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_DIFFUSE));
            globalBindlessDescriptorSet->WriteTexture(BINDLESS_TEXTURE_BINDING, Texture::GetDefaultTexture(TEXTURE_TYPE_SPECULAR));
            globalBindlessDescriptorSet->Allocate();

            VulkanCore::SetGlobalBindlessDescriptorSet(globalBindlessDescriptorSet);
        }
    }

}