//
// Created by Nikolay Kanchevski on 29.12.22.
//

#include "Cubemap.h"

#include <stb_image.h>

#include "../VK.h"
#include "Buffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Cubemap::Cubemap(const CubemapCreateInfo &createInfo)
        : cubemapType(createInfo.cubemapType)
    {
        stbi_uc *stbiImages[6];
        for (uint32_t i = 6; i--;)
        {
            // Load image data
            stbiImages[i] = stbi_load(createInfo.filePaths[i], &width, &height, &colorChannelsCount, STBI_rgb_alpha);

            // Check if image loading has been successful
            ASSERT_ERROR_IF(!stbiImages[i], "Failed to load the cubemap file [" + std::string(createInfo.filePaths[i]) + "]");
        }

        // Create sampler
        this->sampler = Sampler::Create(createInfo.samplerCreateInfo);

        // Calculate the image's memory size
        // TODO: color channels count
        this->layerSize = width * height * 4;
        this->memorySize = layerSize * 6;

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = memorySize,
            .memoryFlags = MEMORY_FLAGS_HOST_VISIBLE | MEMORY_FLAGS_HOST_COHERENT,
            .bufferUsage = TRANSFER_SRC_BUFFER
        });

        // Map memory to buffer
        void *data;
        vmaMapMemory(VK::GetMemoryAllocator(), stagingBuffer->GetMemory(), &data);

        // Copy all 6 images to buffer
        for (uint32_t i = 6; i--;)
        {
            uint64_t memoryAddress = reinterpret_cast<uint64_t>(data) + (layerSize * i);
            memcpy(reinterpret_cast<void*>(memoryAddress), static_cast<void*>(stbiImages[i]), static_cast<size_t>(layerSize));
        }

        // Unmap buffer memory
        vmaUnmapMemory(VK::GetMemoryAllocator(), stagingBuffer->GetMemory());

        // Configure texture format
        ImageFormat textureImageFormat = FORMAT_R8G8B8A8_SRGB;

        // Create the texture image
        this->image = Image::Create({
            .dimensions = { static_cast<uint32_t>(width), static_cast<uint32_t>(height), 1 },
            .imageType = VK_IMAGE_TYPE_2D,
            .format = textureImageFormat,
            .layerCount = 6,
            .usageFlags = TRANSFER_SRC_IMAGE | TRANSFER_DST_IMAGE | SAMPLED_IMAGE,
            .createFlags = IMAGE_FLAGS_CUBE_COMPATIBLE,
            .memoryFlags = MEMORY_FLAGS_DEVICE_LOCAL
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(LAYOUT_TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyImage(*image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (createInfo.mipMappingEnabled) /* GenerateMipMaps() */;

        // NOTE: Transitioning to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL is not required as it is automatically done during the mip map generation
        else image->TransitionLayout(LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Create the image view using the proper image format
        image->CreateImageView(ASPECT_COLOR, VK_IMAGE_VIEW_TYPE_CUBE);

        // Clean up stbi data
        for (const auto &stbiImage : stbiImages)
        {
            stbi_image_free(stbiImage);
        }
    }

    std::unique_ptr<Cubemap> Cubemap::Create(CubemapCreateInfo createInfo)
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