//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../VK.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    Texture::Texture(const TextureCreateInfo &createInfo)
        : textureType(createInfo.textureType), mipMappingEnabled(createInfo.enableMipMapping), isDefault(createInfo.setDefaultTexture)
    {
        // Load image data
        int width, height, channelCount;
        stbi_uc *stbImage = stbi_load(createInfo.filePath.c_str(), &width, &height, &channelCount, GetChannelCountForImageFormat(createInfo.imageFormat));

        // Check if image loading has been successful
        ASSERT_ERROR_FORMATTED_IF(!stbImage, "Failed to load the binary of texture file: {0}", stbi_failure_reason());

        // Save color channel count
        channels = static_cast<TextureChannels>(channelCount);

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = width * height * GetChannelCountForImageFormat(createInfo.imageFormat),
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Copy the image data to the staging buffer
        stagingBuffer->CopyFromPointer(stbImage);
        stbi_image_free(stbImage);

        // Create the texture image
        image = Image::Create({
            .dimensions = { static_cast<uint>(width), static_cast<uint>(height) },
            .format = createInfo.imageFormat,
            .generateMipMaps = createInfo.enableMipMapping,
            .usage = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyToImage(image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (mipMappingEnabled) image->GenerateMipMaps();
        image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create sampler
        SamplerCreateInfo samplerCreateInfo = std::move(createInfo.samplerCreateInfo);
        if (mipMappingEnabled) samplerCreateInfo.maxLod = static_cast<float>(image->GetMipMapLevels());
        sampler = Sampler::Create(samplerCreateInfo);
    }

    SharedPtr<Texture> Texture::Create(const TextureCreateInfo &createInfo)
    {
        // Check if the texture file has already been loaded to texture
        auto iterator = texturePool.find(HashType(createInfo.filePath));
        if (iterator != texturePool.end())
        {
            // If the same texture file has been used check to see if its sampler is the same as this one
            auto &otherSampler = iterator->second->GetSampler();
            if (
                otherSampler->IsBilinearFilteringApplied() == createInfo.samplerCreateInfo.applyBilinearFiltering &&
                otherSampler->GetMinLod() == createInfo.samplerCreateInfo.minLod &&
                otherSampler->GetMaxLod() == createInfo.samplerCreateInfo.maxLod &&
                otherSampler->IsAnisotropyEnabled() == createInfo.samplerCreateInfo.enableAnisotropy &&
                otherSampler->GetAddressMode() == createInfo.samplerCreateInfo.addressMode
            )
            {
                // If so return it without creating a new texture
                auto &foundTexture = iterator->second;
                if (createInfo.setDefaultTexture)
                {
                    defaultTextures[static_cast<uint>(createInfo.textureType)] = foundTexture;
                }
                return foundTexture;
            }
        }

        // Check if texture is supposed to be default
        auto &textureReference = texturePool[HashType(createInfo.filePath)];
        textureReference = std::make_shared<Texture>(createInfo);
        if (createInfo.setDefaultTexture)
        {
            ASSERT_ERROR_FORMATTED_IF(createInfo.textureType == TextureType::UNDEFINED, "Cannot set texture loaded from [{0}] as default for its type, as it is of type TextureType::UNDEFINED", createInfo.filePath);
            defaultTextures[static_cast<uint>(createInfo.textureType)] = textureReference;
        }

        return textureReference;
    }

    Texture::Texture(const BinaryTextureCreateInfo &createInfo)
        : textureType(createInfo.textureType), channels(createInfo.channels), mipMappingEnabled(createInfo.enableMipMapping), isDefault(createInfo.setDefaultTexture)
    {
        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = createInfo.width * createInfo.height * GetChannelCountForImageFormat(createInfo.imageFormat),
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Copy the image data to the staging buffer
        stagingBuffer->CopyFromPointer(createInfo.data);

        // Create the texture image
        image = Image::Create({
            .dimensions = { createInfo.width, createInfo.height },
            .format = createInfo.imageFormat,
            .generateMipMaps = createInfo.enableMipMapping,
            .usage = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
        });

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(ImageLayout::TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyToImage(image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (mipMappingEnabled) image->GenerateMipMaps();
        image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create sampler
        SamplerCreateInfo samplerCreateInfo = std::move(createInfo.samplerCreateInfo);
        if (mipMappingEnabled) samplerCreateInfo.maxLod = static_cast<float>(image->GetMipMapLevels());
        sampler = Sampler::Create(samplerCreateInfo);
    }

    SharedPtr<Texture> Texture::Load(const BinaryTextureCreateInfo &createInfo)
    {
        // Check if texture is supposed to be default
        auto texture = std::make_shared<Texture>(createInfo);
        if (createInfo.setDefaultTexture)
        {
            ASSERT_ERROR_IF(createInfo.textureType == TextureType::UNDEFINED, "Cannot set binary texture as default for its type, as it is of type TextureType::UNDEFINED");
            defaultTextures[static_cast<uint>(createInfo.textureType)] = texture;
        }

        return texture;
    }

    /* --- SETTER METHODS --- */

    void Texture::Dispose()
    {
        // Remove texture from pool
        texturePool.erase(HashType(filePath));
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
        if (imGuiDescriptorSet == nullptr) imGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(sampler->GetVulkanSampler(), image->GetVulkanImageView(), static_cast<VkImageLayout>(image->GetLayout()));
        return imGuiDescriptorSet;
    }

    /* --- DESTRUCTOR --- */

    void Texture::Destroy()
    {
        image->Destroy();
        sampler->Destroy();

        if (imGuiDescriptorSet != nullptr) VK_VALIDATE(vkFreeDescriptorSets(VK::GetLogicalDevice(), VK::GetImGuiDescriptorPool(), 1,reinterpret_cast<VkDescriptorSet const *>(&imGuiDescriptorSet)), "Could not free ImGui texture");
        Dispose();
    }
}