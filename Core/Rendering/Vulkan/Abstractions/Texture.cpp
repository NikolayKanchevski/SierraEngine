//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../VK.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Texture::Texture(stbi_uc *stbImage, const uint width, const uint height, const uint givenColorChannelsCount, TextureCreateInfo &createInfo)
        : textureType(createInfo.textureType), mipMappingEnabled(createInfo.mipMappingEnabled), colorChannelsCount(givenColorChannelsCount)
    {
        // Calculate the image's memory size
        this->memorySize = width * height * GetChannelCountForImageFormat(createInfo.imageFormat);

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = memorySize,
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Copy the image data to the staging buffer
        stagingBuffer->CopyFromPointer(stbImage);
        stbi_image_free(stbImage);

        // Create the texture image
        this->image = Image::Create({
            .dimensions = { width, height },
            .format = createInfo.imageFormat,
            .generateMipMaps = createInfo.mipMappingEnabled,
            .usage = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyToImage(*image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (mipMappingEnabled) image->GenerateMipMaps();
        image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create sampler
        if (mipMappingEnabled) createInfo.samplerCreateInfo.maxLod = static_cast<float>(image->GetMipMapLevels());
        this->sampler = Sampler::Create(createInfo.samplerCreateInfo);
    }

    SharedPtr<Texture> Texture::Create(TextureCreateInfo createInfo, const bool setDefaultTexture)
    {
        // Check if the texture file has already been loaded to texture
        Hash hash = GET_HASH(createInfo.filePath);
        if (texturePool.count(hash) != 0)
        {
            // If the same texture file has been used check to see if its sampler is the same as this one
            Sampler *other = (&texturePool[hash]->GetSampler())->get();
            if (other->IsBilinearFilteringApplied() == createInfo.samplerCreateInfo.applyBilinearFiltering && other->GetMinLod() == createInfo.samplerCreateInfo.minLod && other->GetMaxLod() == createInfo.samplerCreateInfo.maxLod && other->IsAnisotropyEnabled() == createInfo.samplerCreateInfo.enableAnisotropy && other->GetAddressMode() == createInfo.samplerCreateInfo.addressMode)
            {
                // If so return it without creating a new texture
                auto &foundTexture = texturePool[hash];
                defaultTextures[static_cast<uint>(createInfo.textureType)] = foundTexture;
                return foundTexture;
            }
        }

        // Number of channels texture has
        int channels;

        // Load image data
        int width, height;
        stbi_uc *stbiImage = stbi_load(createInfo.filePath.c_str(), &width, &height, &channels, GetChannelCountForImageFormat(createInfo.imageFormat));

        // Check if image loading has been successful
        ASSERT_ERROR_FORMATTED_IF(!stbiImage, "Failed to load the texture file [{0}]", createInfo.filePath);

        // If texture does not exist already
        auto &textureReference = texturePool[GET_HASH(createInfo.filePath)];
        textureReference = std::make_shared<Texture>(stbiImage, width, height, channels, createInfo);
        if (setDefaultTexture)
        {
            ASSERT_ERROR_FORMATTED_IF(createInfo.textureType == TextureType::UNDEFINED_TEXTURE, "Cannot set texture loaded from [{0}] as default texture for its type, as it is of type TEXTURE_TYPE_NONE", createInfo.filePath);

            textureReference->isDefault = true;
            defaultTextures[createInfo.textureType] = textureReference;
        }

        return textureReference;
    }

    /* --- SETTER METHODS --- */

    void Texture::Dispose()
    {
        // Remove texture from pool
        texturePool.erase(GET_HASH(filePath));
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
        for (const auto &texture : defaultTextures)
        {
            texture->Destroy();
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

        Dispose();
    }
}