//
// Created by Nikolay Kanchevski on 29.12.22.
//

#include "Cubemap.h"

#include "../VK.h"
#include "Buffer.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    Cubemap::Cubemap(const CubemapCreateInfo &createInfo)
        : cubemapType(createInfo.cubemapType)
    {
        stbi_uc *stbiImages[6];

        uint channelCount = GetChannelCountForImageFormat(createInfo.imageFormat);
        for (uint i = 6; i--;)
        {
            // Load image data
            filePaths[i] = createInfo.filePaths[i];
            stbiImages[i] = stbi_load(createInfo.filePaths[i].c_str(), &width, &height, &colorChannelsCount, channelCount);

            // Check if image loading has been successful
            ASSERT_ERROR_FORMATTED_IF(!stbiImages[i], "Failed to load the cubemap file [{0}]", createInfo.filePaths[i]);
        }

        // Create sampler
        sampler = Sampler::Create(createInfo.samplerCreateInfo);

        // Calculate the image's memory size
        uint64 layerSize = width * height * channelCount;
        memorySize = layerSize * 6;

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = memorySize,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Map memory to buffer
        void *data;
        vmaMapMemory(VK::GetMemoryAllocator(), stagingBuffer->GetMemoryAllocation(), &data);

        // Copy all 6 images to buffer
        for (uint i = 6; i--;)
        {
            uint64 memoryAddress = reinterpret_cast<uint64>(data) + (layerSize * i);
            memcpy(reinterpret_cast<void*>(memoryAddress), static_cast<void*>(stbiImages[i]), static_cast<uSize>(layerSize));
        }

        // Unmap buffer memory
        vmaUnmapMemory(VK::GetMemoryAllocator(), stagingBuffer->GetMemoryAllocation());

        // Create the texture image
        image = Image::Create({
            .dimensions = { static_cast<uint>(width), static_cast<uint>(height), 1 },
            .imageType = ImageType::TEXTURE,
            .format = createInfo.imageFormat,
            .layerCount = 6,
            .usage = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
            .createFlags = ImageCreateFlags::CUBE_COMPATIBLE,
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyToImage(image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (createInfo.mipMappingEnabled) image->GenerateMipMaps();
        image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Clean up stbi data
        for (const auto &stbiImage : stbiImages)
        {
            stbi_image_free(stbiImage);
        }
    }

    UniquePtr<Cubemap> Cubemap::Create(CubemapCreateInfo createInfo)
    {
        return std::make_unique<Cubemap>(createInfo);
    }

    /* --- DESTRUCTOR --- */

    void Cubemap::Destroy()
    {
        sampler->Destroy();
        image->Destroy();
    }

}