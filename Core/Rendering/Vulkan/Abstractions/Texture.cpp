//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../VK.h"
#include "../../../../Engine/Classes/File.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Texture::Texture(stbi_uc *stbImage, const uint width, const uint height, const uint givenColorChannelsCount, TextureCreateInfo &textureCreateInfo)
        : name(textureCreateInfo.name), filePath(textureCreateInfo.filePath), textureType(textureCreateInfo.textureType), mipMappingEnabled(textureCreateInfo.mipMappingEnabled), colorChannelsCount(givenColorChannelsCount)
    {
        // Calculate the image's memory size
        this->memorySize = width * height * 4;

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = memorySize,
            .memoryFlags = MemoryFlags::HOST_VISIBLE | MemoryFlags::HOST_COHERENT,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Copy the image data to the staging buffer
        stagingBuffer->CopyFromPointer(stbImage);
        stbi_image_free(stbImage);

        // Configure the color format
        ImageFormat textureImageFormat = ImageFormat::R8G8B8A8_SRGB;

        // Create the texture image
        this->image = Image::Create({
            .dimensions = { width, height, 1 },
            .format = textureImageFormat,
            .generateMipMaps = textureCreateInfo.mipMappingEnabled,
            .usageFlags = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
            .memoryFlags = MemoryFlags::DEVICE_LOCAL
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyImage(*image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (mipMappingEnabled) image->GenerateMipMaps();

        // NOTE: Transitioning to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL is not required as it is automatically done during the mip map generation
        else image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create the image view using the proper image format
        image->CreateImageView(ImageAspectFlags::COLOR);

        // Create sampler
        if (mipMappingEnabled) textureCreateInfo.samplerCreateInfo.maxLod = static_cast<float>(image->GetMipMapLevels());
        this->sampler = Sampler::Create(textureCreateInfo.samplerCreateInfo);
    }

    SharedPtr<Texture> Texture::Create(TextureCreateInfo textureCreateInfo, const bool setDefaultTexture)
    {
        // Check if the texture file has already been loaded to texture
        if (texturePool.count(textureCreateInfo.filePath) != 0)
        {
            // If the same texture file has been used check to see if its sampler is the same as this one
            Sampler *other = (&texturePool[textureCreateInfo.filePath]->GetSampler())->get();
            if (other->IsBilinearFilteringApplied() == textureCreateInfo.samplerCreateInfo.applyBilinearFiltering && other->GetMinLod() == textureCreateInfo.samplerCreateInfo.minLod && other->GetMaxLod() == textureCreateInfo.samplerCreateInfo.maxLod  && other->GetMaxAnisotropy() == textureCreateInfo.samplerCreateInfo.maxAnisotropy && other->GetAddressMode() == textureCreateInfo.samplerCreateInfo.samplerAddressMode)
            {
                // If so return it without creating a new texture
                return texturePool[textureCreateInfo.filePath];
            }
        }

        // Set a default tag if none is assigned
        if (textureCreateInfo.name == "") textureCreateInfo.name = Engine::Classes::File::GetFileNameFromPath(textureCreateInfo.filePath);

        // Number of channels texture has
        int channels;

        // Load image data
        int width, height;
        stbi_uc *stbiImage = stbi_load(textureCreateInfo.filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        // Check if image loading has been successful
        ASSERT_ERROR_FORMATTED_IF(!stbiImage, "Failed to load the texture file [{0}]", textureCreateInfo.filePath);

        // If texture does not exist already
        auto &textureReference = texturePool[textureCreateInfo.filePath];
        textureReference = std::make_shared<Texture>(stbiImage, width, height, channels, textureCreateInfo);
        if (setDefaultTexture)
        {
            ASSERT_ERROR_FORMATTED_IF(textureCreateInfo.textureType == TEXTURE_TYPE_NONE, "Cannot set texture loaded from [{0}] as default texture for its type, as it is of type TEXTURE_TYPE_NONE", textureCreateInfo.filePath);

            textureReference->isDefault = true;
            defaultTextures[textureCreateInfo.textureType] = textureReference;
        }

        return textureReference;
    }

    /* --- SETTER METHODS --- */

    void Texture::Dispose()
    {
        // Remove texture from pool
        texturePool.erase(filePath);
    }

    void Texture::DisposePool()
    {
        for (const auto &texture : texturePool)
        {
            if (texture.second->isDefault) continue;

            texture.second->Destroy();
        }

        texturePool.clear();
    }

    void Texture::DestroyDefaultTextures()
    {
        for (const auto &texture : texturePool)
        {
            texture.second->Destroy();
        }
    }

    /* --- GETTER METHODS --- */

    ImTextureID Texture::GetImGuiTextureID()
    {
        // Create ImGui descriptor set if not created
        if (!imGuiDescriptorSetCreated)
        {
            imGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(sampler->GetVulkanSampler(), image->GetVulkanImageView(), (VkImageLayout) image->GetLayout());
            imGuiDescriptorSetCreated = true;
        }

        return (ImTextureID) imGuiDescriptorSet;
    }

    /* --- DESTRUCTOR --- */

    void Texture::Destroy()
    {
        image->Destroy();
        sampler->Destroy();
    }
}