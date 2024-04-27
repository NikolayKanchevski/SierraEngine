//
// Created by Nikolay Kanchevski on 23.04.24.
//

#include "Texture.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Texture::Texture(const TextureCreateInfo &createInfo)
        : type(createInfo.importedTexture.type), image(std::move(createInfo.importedTexture.image))
    {
        // Write every level to image
        createInfo.commandBuffer->SynchronizeImageUsage(image, Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::MemoryWrite);
        for (uint32 level = 0; level < image->GetLevelCount(); level++)
        {
            std::unique_ptr<Sierra::Buffer> &levelBuffer = createInfo.importedTexture.levelBuffers[level];
            for (uint32 layer = 0; layer < image->GetLayerCount(); layer++)
            {
                createInfo.commandBuffer->CopyBufferToImage(levelBuffer, image, level, layer, { 0, 0 }, layer * (levelBuffer->GetMemorySize() / image->GetLayerCount()));
            }
            createInfo.commandBuffer->QueueBufferForDestruction(std::move(levelBuffer));
        }
        createInfo.commandBuffer->SynchronizeImageUsage(image, Sierra::ImageCommandUsage::MemoryWrite, Sierra::ImageCommandUsage::GraphicsRead);
    }

}