//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Texture.h"
#include "Buffer.h"
#include "../VulkanCore.h"
#include "../VulkanUtilities.h"
#include <glm/common.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    std::unordered_map<std::string, std::shared_ptr<Texture>> Texture::texturePool;

    /* --- CONSTRUCTORS --- */
    Texture::Texture(stbi_uc *stbImage, const std::string givenFilePath, const uint32_t width, const uint32_t height, const TextureType givenTextureType, const int givenColorChannelsCount, const uint64_t givenMemorySize, const bool givenMipMappingEnabled, Sampler::Builder &samplerBuilder, std::unique_ptr<DescriptorSetLayout> &givenDescriptorSetLayout, std::unique_ptr<DescriptorPool> &givenDescriptorPool, const std::string givenName)
        : name(givenName), filePath(givenFilePath), textureType(givenTextureType), sampler(samplerBuilder.Build()), colorChannelsCount(givenColorChannelsCount), memorySize(givenMemorySize)
    {
        // If mip mapping is enabled calculate mip levels
        if (givenMipMappingEnabled)
        {
            this->mipMappingEnabled = true;
            mipMapLevels = static_cast<uint32_t>(glm::floor(std::log2(glm::max(width, height)) + 1));
        }

        // Calculate the image's memory size
        this->memorySize = static_cast<uint32_t>(width * height * 4);

        // Create the staging buffer
        auto stagingBuffer = Buffer::Builder()
            .SetMemorySize(memorySize)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
            .SetUsageFlags(VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
        .Build();

        // Copy the image data to the staging buffer
        stagingBuffer->CopyFromPointer(stbImage);
        stbi_image_free(stbImage);

        // Configure the color format
        VkFormat textureImageFormat = VK_FORMAT_R32_UINT;
        switch (colorChannelsCount)
        {
            case STBI_rgb_alpha:
            case STBI_rgb:
                textureImageFormat = VK_FORMAT_R8G8B8A8_SRGB;
                break;
            default:
                VulkanDebugger::ThrowError("Texture format not supported");
                break;
        }

        // Create the texture image
        this->image = Image::Builder()
            .SetWidth(width)
            .SetHeight(height)
            .SetMipLevels(mipMapLevels)
            .SetFormat(textureImageFormat)
            .SetUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
            .SetMemoryFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
        .Build();

        // Transition the layout of the image, so it can be used for copying
        image->TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        // Copy the image to the staging buffer
        stagingBuffer->CopyImage(*image);

        // Destroy the staging buffer and free its memory
        stagingBuffer->Destroy();

        // Generate mip maps for the current texture
        if (mipMappingEnabled) GenerateMipMaps();
        // NOTE: Transitioning to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL is not required as it is automatically done during the mip map generation
        else image->TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        // Create the image view using the proper image format
        image->CreateImageView(VK_IMAGE_ASPECT_COLOR_BIT);
    }

    Texture::Builder::Builder(std::unique_ptr<DescriptorPool> &givenDescriptorPool)
        : descriptorPool(givenDescriptorPool)
    {

    }

    Texture::Builder &Texture::Builder::SetName(const std::string givenName)
    {
        // Save given name
        this->name = givenName;
        return *this;
    }

    Texture::Builder &Texture::Builder::SetTextureType(const TextureType givenTextureType)
    {
        // Save given texture type
        this->textureType = givenTextureType;
        return *this;
    }

    Texture::Builder &Texture::Builder::EnableMipMapGeneration(const bool isApplied)
    {
        // Save given mip generation toggle
        mipMappingEnabled = isApplied;
        return *this;
    }

    Texture::Builder &Texture::Builder::SetMaxAnisotropy(float givenMaxAnisotropy)
    {
        // Check if sampler anisotropy is supported by the GPU
        if (VulkanCore::GetPhysicalDeviceFeatures().samplerAnisotropy)
        {
            // Clamp the anisotropy between 0.0 and 1.0 and multiply it by the maximum supported anisotropy
            givenMaxAnisotropy = Math::Clamp(givenMaxAnisotropy, 0.0f, 1.0f);
            this->maxAnisotropy = (givenMaxAnisotropy / 1.0f) * VulkanCore::GetPhysicalDeviceProperties().limits.maxSamplerAnisotropy;
        }
        else
        {
            VulkanDebugger::ThrowWarning("Sampler anisotropy is requested but not supported by the GPU. The feature has automatically been disabled");
        }

        return *this;
    }

    Texture::Builder &Texture::Builder::SetAddressMode(const VkSamplerAddressMode givenAddressMode)
    {
        // Save the provided address mode
        samplerAddressMode = givenAddressMode;
        return *this;
    }

    Texture::Builder &Texture::Builder::SetLod(const glm::vec2 givenLod)
    {
        // Save the provided LOD values
        minLod = givenLod.x;
        maxLod = givenLod.y;
        return *this;
    }

    Texture::Builder &Texture::Builder::ApplyBilinearFiltering(const bool isApplied)
    {
        // Save the bilinear filtering preference
        applyBilinearFiltering = isApplied;
        return *this;
    }

    std::shared_ptr<Texture> Texture::Builder::Build(const std::string givenFilePath)
    {
        // Check if the texture file has already been loaded to texture
        if (texturePool.count(givenFilePath) != 0)
        {
            // If the same texture file has been used check to see if its sampler is the same as this one
            Sampler *other = (&texturePool[givenFilePath]->GetSampler())->get();
            if (other->IsBilinearFilteringApplied() == applyBilinearFiltering && other->GetMinLod() == minLod && other->GetMaxLod() == maxLod  && other->GetMaxAnisotropy() == maxAnisotropy && other->GetAddressMode() == samplerAddressMode)
            {
                // If so return it without creating a new texture
                return texturePool[givenFilePath];
            }
        }

        // Set a default name if none is assigned
        if (name == "") this->name = givenFilePath;

        // Number of channels texture has
        int channels;

        // Load image data
        int width, height;
        stbi_uc *stbiImage = stbi_load(givenFilePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        // Check if image loading has been successful
        if (!stbiImage) VulkanDebugger::ThrowError("Failed to load the texture file [" + givenFilePath + "]");

        // Calculate memory size for image
        const uint64_t calculatedMemorySize = width * height * channels;

        // If texture does not exist already
        auto texture = std::make_shared<Texture>(stbiImage, givenFilePath, width, height, textureType, channels, calculatedMemorySize, mipMappingEnabled, Sampler::Builder().SetLod({minLod, maxLod }).SetAddressMode(samplerAddressMode).SetMaxAnisotropy(maxAnisotropy).ApplyBilinearFiltering(applyBilinearFiltering), descriptorPool->GetDescriptorSetLayout(), descriptorPool, name);
        texturePool[givenFilePath] = texture;
        return texture;
    }

    /* --- SETTER METHODS --- */

    void Texture::GenerateMipMaps()
    {
        // Get the properties of the image's format
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(VulkanCore::GetPhysicalDevice(), image->GetFormat(), &formatProperties);

        // Check if optimal tiling is supported by the GPU
        if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == 0)
        {
            VulkanDebugger::ThrowError("Texture image format [" + std::to_string(image->GetFormat()) + "] does not support linear blitting");
        }

        // Begin a command buffer
        VkCommandBuffer commandBuffer = VulkanUtilities::BeginSingleTimeCommands();

        // Create an image memory barrier
        VkImageMemoryBarrier memoryBarrier{};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        memoryBarrier.image = image->GetVulkanImage();
        memoryBarrier.srcQueueFamilyIndex = ~0U;
        memoryBarrier.dstQueueFamilyIndex = ~0U;
        memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        memoryBarrier.subresourceRange.baseArrayLayer = 0;
        memoryBarrier.subresourceRange.layerCount = 1;
        memoryBarrier.subresourceRange.levelCount = 1;

        int mipWidth = GetWidth();
        int mipHeight = GetHeight();

        // For each mip level resize the image
        for (uint32_t i = 1; i < mipMapLevels; i++) {
            memoryBarrier.subresourceRange.baseMipLevel = i - 1;
            memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            memoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            memoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(
                    commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &memoryBarrier
            );

            VkImageBlit blit{};
            blit.srcOffsets[0] = { 0, 0, 0 };
            blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = { 0, 0, 0 };
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(
                commandBuffer,
                image->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                image->GetVulkanImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR
            );

            memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(
                commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &memoryBarrier
            );

            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }

        // Set base mip level and transition the layout of the texture
        memoryBarrier.subresourceRange.baseMipLevel = mipMapLevels - 1;
        memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        memoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        // Bind the image barrier and apply the changes
        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &memoryBarrier
          );

        // End the current command buffer
        VulkanUtilities::EndSingleTimeCommands(commandBuffer);
    }

    /* --- DESTRUCTOR --- */
    void Texture::Destroy()
    {
        texturePool.erase(filePath);

        image->Destroy();
        sampler->Destroy();
    }
}