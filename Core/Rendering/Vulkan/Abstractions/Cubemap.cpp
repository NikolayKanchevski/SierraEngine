//
// Created by Nikolay Kanchevski on 29.12.22.
//

#include "Cubemap.h"


#include "../VK.h"
#include "Buffer.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Cubemap::Cubemap(const CubemapCreateInfo &createInfo)
        : cubemapType(createInfo.cubemapType)
    {
        stbi_uc *stbiImages[6];
        for (uint i = 6; i--;)
        {
            // Load image data
            stbiImages[i] = stbi_load(createInfo.filePaths[i], &width, &height, &colorChannelsCount, STBI_rgb_alpha);

            // Check if image loading has been successful
            ASSERT_ERROR_FORMATTED_IF(!stbiImages[i], "Failed to load the cubemap file [{0}]", createInfo.filePaths[i]);
        }

        // Create sampler
        this->sampler = Sampler::Create(createInfo.samplerCreateInfo);

        // Calculate the image's memory size
        this->layerSize = width * height * 4;
        this->memorySize = layerSize * 6;

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = memorySize,
            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Map memory to buffer
        void *data;
        vmaMapMemory(VK::GetMemoryAllocator(), stagingBuffer->GetMemory(), &data);

        // Copy all 6 images to buffer
        for (uint i = 6; i--;)
        {
            uint64 memoryAddress = reinterpret_cast<uint64>(data) + (layerSize * i);
            memcpy(reinterpret_cast<void*>(memoryAddress), static_cast<void*>(stbiImages[i]), static_cast<uSize>(layerSize));
        }

        // Unmap buffer memory
        vmaUnmapMemory(VK::GetMemoryAllocator(), stagingBuffer->GetMemory());

        // Configure texture format
        ImageFormat textureImageFormat = ImageFormat::R8G8B8A8_SRGB;

        // Create the texture image
        this->image = Image::Create({
            .dimensions = { static_cast<uint>(width), static_cast<uint>(height), 1 },
            .imageType = ImageType::TEXTURE,
            .format = textureImageFormat,
            .layerCount = 6,
            .usageFlags = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
            .createFlags = ImageCreateFlags::CUBE_COMPATIBLE,
            .memoryFlags = MemoryFlags::DEVICE_LOCAL
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyImage(*image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (createInfo.mipMappingEnabled) /* GenerateMipMaps() */;

        // NOTE: Transitioning to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL is not required as it is automatically done during the mip map generation
        else image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create the image view using the proper image format
        image->CreateImageView(ImageAspectFlags::COLOR, VK_IMAGE_VIEW_TYPE_CUBE);

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