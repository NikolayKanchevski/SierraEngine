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

    Texture::Texture(stbi_uc *stbImage, const uint width, const uint height, const uint givenColorChannelsCount, const TextureCreateInfo& textureCreateInfo)
        : name(textureCreateInfo.name), filePath(textureCreateInfo.filePath), textureType(textureCreateInfo.textureType), mipMappingEnabled(textureCreateInfo.mipMappingEnabled), colorChannelsCount(givenColorChannelsCount)
    {
        // Create sampler
        this->sampler = Sampler::Create(textureCreateInfo.samplerCreateInfo);

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
        if (mipMappingEnabled) GenerateMipMaps();

        // NOTE: Transitioning to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL is not required as it is automatically done during the mip map generation
        else image->TransitionLayout(ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create the image view using the proper image format
        image->CreateImageView(ImageAspectFlags::COLOR);
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
        ASSERT_ERROR_IF(!stbiImage, "Failed to load the texture file [" + textureCreateInfo.filePath + "]");

        // If texture does not exist already
        auto &textureReference = texturePool[textureCreateInfo.filePath];
        textureReference = std::make_shared<Texture>(stbiImage, width, height, channels, textureCreateInfo);
        if (setDefaultTexture)
        {
            ASSERT_ERROR_IF(textureCreateInfo.textureType == TEXTURE_TYPE_NONE, "Cannot set texture loaded from [" + textureCreateInfo.filePath + "] as default texture for its type, as it is of type TEXTURE_TYPE_NONE");

            textureReference->isDefault = true;
            defaultTextures[textureCreateInfo.textureType] = textureReference;
        }

        return textureReference;
    }

    /* --- SETTER METHODS --- */

    void Texture::GenerateMipMaps()
    {
        if (GetWidth() + GetHeight() < 1024)
        {
            ASSERT_WARNING("Image is too small for mip map generation. Its size is " + std::to_string(GetWidth()) + "x" + std::to_string(GetHeight()) + ", while an image of total size bigger than 512x512 is required. Action suspended automatically");
            return;
        }

        mipMapLevels = static_cast<uint>(glm::floor(std::log2(glm::max(GetWidth(), GetHeight())) + 1));

        // Get the properties of the image's format
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(VK::GetPhysicalDevice(), (VkFormat) image->GetFormat(), &formatProperties);

        // Check if optimal tiling is supported by the GPU
        ASSERT_ERROR_IF((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) == 0, "Texture image format [" + std::to_string((int) image->GetFormat()) + "] does not support linear blitting");

        // Begin a command buffer
        VkCommandBuffer commandBuffer = VK::GetDevice()->BeginSingleTimeCommands();

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
        for (uint i = 1; i < mipMapLevels; i++)
        {
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
        VK::GetDevice()->EndSingleTimeCommands(commandBuffer);
    }

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