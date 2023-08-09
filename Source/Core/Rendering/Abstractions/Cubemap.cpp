//
// Created by Nikolay Kanchevski on 29.12.22.
//

#include "Cubemap.h"

#include "../Bases/VK.h"
#include "Buffer.h"
#include "../../../Engine/Handlers/Assets/AssetManager.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    Cubemap::Cubemap(const BinaryCubemapCreateInfo &createInfo)
    {
        // Get command buffer to use to load resources to GPU
        auto &commandBuffer = Engine::AssetManager::GetCommandBuffer();

        // Calculate single image's memory size
        uint64 layerSize = createInfo.width * createInfo.height * 4;

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = layerSize * 6,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Copy all 6 images to buffer
        for (uint32 i = 6; i--;)
        {
            stagingBuffer->CopyFromPointer(createInfo.data[i], layerSize, layerSize * i);
        }

        // Create the texture image
        image = Image::Create({
            .width = createInfo.width,
            .height = createInfo.height,
            .imageType = ImageType::TEXTURE,
            .format = CreateImageFormat(createInfo.channels == ImageChannels::RGB ? ImageChannels::RGBA : createInfo.channels, createInfo.memoryType),
            .layerCount = 6,
            .usage = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
            .createFlags = ImageCreateFlags::CUBE_COMPATIBLE,
        });

        // Prepare image for copying data to it
        commandBuffer->SynchronizeImageUsage(image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        commandBuffer->CopyBufferToImage(stagingBuffer, image);
        commandBuffer->DestroyBufferAfterExecution(stagingBuffer);

        // Generate mip maps for the texture
        commandBuffer->SynchronizeImageUsage(image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create sampler
        sampler = Sampler::Create({
            .applyBilinearFiltering = createInfo.enableSmoothFiltering,
            .enableAnisotropy = false
        });
    }

    SharedPtr<Cubemap> Cubemap::Load(const BinaryCubemapCreateInfo &createInfo)
    {
        return std::make_shared<Cubemap>(createInfo);
    }

    SharedPtr<Cubemap> Cubemap::Create(const CubemapCreateInfo &createInfo)
    {
        // Load images data
        stbi_uc* stbiImages[6];
        int32 width, height, channelCount;
        for (uint32 i = 6; i--;)
        {
            stbiImages[i] = stbi_load(createInfo.filePaths[i].c_str(), &width, &height, &channelCount, 4);
            ASSERT_ERROR_IF(stbiImages[i] == nullptr, "Failed to load the cubemap file [{0}]", createInfo.filePaths[i]);
        }

        // Create cubemap
        auto cubemap = Cubemap::Load({
            .data = { stbiImages[0], stbiImages[1], stbiImages[2], stbiImages[3], stbiImages[4], stbiImages[5] },
            .width = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .channels = static_cast<ImageChannels>(channelCount),
            .memoryType = createInfo.memoryType,
            .enableSmoothFiltering = createInfo.enableSmoothFiltering
        });

        // Free image data
        for (uint32 i = 6; i--;)
        {
            stbi_image_free(stbiImages[i]);
        }

        return cubemap;
    }

    /* --- DESTRUCTOR --- */

    void Cubemap::Destroy()
    {
        sampler->Destroy();
        image->Destroy();
    }

}