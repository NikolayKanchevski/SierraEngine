//
// Created by Nikolay Kanchevski on 30.09.22.
//

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../Bases/VK.h"
#include "../../../Engine/Handlers/Assets/AssetManager.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    Texture::Texture(const BinaryTextureCreateInfo &createInfo)
    {
        // Get command buffer to use to load resources to GPU
        auto &commandBuffer = Engine::AssetManager::GetCommandBuffer();

        // Create the staging buffer
        auto stagingBuffer = Buffer::Create({
            .memorySize = createInfo.width * createInfo.height * static_cast<uint>(createInfo.channels == ImageChannels::RGB ? ImageChannels::RGBA : createInfo.channels),
            .bufferUsage = BufferUsage::TRANSFER_SRC
        });

        // Copy the image data to the staging buffer
        stagingBuffer->CopyFromPointer(createInfo.data);

        // Create the texture image
        image = Image::Create({
            .width = createInfo.width,
            .height = createInfo.height,
            .format = CreateImageFormat(createInfo.channels == ImageChannels::RGB ? ImageChannels::RGBA : createInfo.channels, createInfo.memoryType),
            .mipLevels = createInfo.generateMipMaps ? static_cast<uint32>(glm::floor(std::log2(glm::max(createInfo.width, createInfo.height)))) + 1 : 1,
            .usage = ImageUsage::TRANSFER_SRC | ImageUsage::TRANSFER_DST | ImageUsage::SAMPLED,
        });

        // Prepare image for copying data to it
        commandBuffer->SynchronizeImageUsage(image, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_ACCESS_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, ImageLayout::TRANSFER_DST_OPTIMAL);

        // Insert image data into the image
        commandBuffer->CopyBufferToImage(stagingBuffer, image);
        commandBuffer->DestroyBufferAfterExecution(stagingBuffer);

        // Generate mip maps if needed and transition image to be optimal for sampling in shader
        if (createInfo.generateMipMaps) GenerateMipMaps();
        commandBuffer->SynchronizeImageUsage(image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, ImageLayout::SHADER_READ_ONLY_OPTIMAL);

        // Create sampler
        sampler = Sampler::Create({
            .maxLod = createInfo.generateMipMaps ? static_cast<float>(image->GetMipLevels()) : 0,
            .applyBilinearFiltering = createInfo.enableSmoothFiltering,
            .enableAnisotropy = true
        });
    }

    SharedPtr<Texture> Texture::Load(const BinaryTextureCreateInfo &createInfo)
    {
        return std::make_shared<Texture>(createInfo);
    }

    SharedPtr<Texture> Texture::Create(const TextureCreateInfo &createInfo)
    {
        // Load image data
        int32 width, height, channelCount;
        stbi_uc* stbImage = stbi_load(createInfo.filePath.c_str(), &width, &height, &channelCount, 4);

        // Create texture
        auto texture = Texture::Load({
            .data = stbImage,
            .width = static_cast<uint32>(width),
            .height = static_cast<uint32>(height),
            .channels = static_cast<ImageChannels>(channelCount),
            .memoryType = createInfo.memoryType,
            .enableSmoothFiltering = createInfo.enableSmoothFiltering,
            .generateMipMaps = createInfo.generateMipMaps
        });

        // Free image data
        stbi_image_free(stbImage);

        return texture;
    }

    /* --- GETTER METHODS --- */

    ImTextureID Texture::GetImGuiTextureID()
    {
        // Create ImGui descriptor set if not created
        if (imGuiDescriptorSet == nullptr) imGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(sampler->GetVulkanSampler(), image->GetVulkanImageView(), static_cast<VkImageLayout>(ImageLayout::SHADER_READ_ONLY_OPTIMAL));
        return imGuiDescriptorSet;
    }

    /* --- PRIVATE METHODS --- */

    void Texture::GenerateMipMaps() const
    {
        // Check if texture is not too small (would make mip mapping pointless)
        if (GetWidth() * GetHeight() < 256 * 256)
        {
            ASSERT_WARNING("Image is too small for mip map generation. Its size is {0}x{1}, while an image of total size bigger than 512x512 is required. Action suspended automatically", GetWidth(), GetHeight());
            return;
        }

        // Get asset manager's command buffer
        auto &commandBuffer = Engine::AssetManager::GetCommandBuffer();

        // Set initial mip sizes
        uint32 mipWidth = image->GetWidth();
        uint32 mipHeight = image->GetHeight();
        for (uint32_t i = 0; i < image->GetMipLevels() - 1; i++)
        {
            // Make current level copy-optimal (we will be copying current level into next level, whilst lowering the resolution) and blit
            commandBuffer->SynchronizeImageLevelUsage(image, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT, i, ImageLayout::TRANSFER_SRC_OPTIMAL);
            commandBuffer->BlitImage(image, { mipWidth, mipHeight }, { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1 }, i);

            // Calculate next mip level's dimension (they are twice as low-res as current)
            if (mipWidth > 1) mipWidth /= 2;
            if (mipHeight > 1) mipHeight /= 2;
        }
    }

    /* --- DESTRUCTOR --- */

    void Texture::Destroy()
    {
        image->Destroy();
        sampler->Destroy();

        if (imGuiDescriptorSet != nullptr) VK_VALIDATE(vkFreeDescriptorSets(VK::GetLogicalDevice(), VK::GetImGuiDescriptorPool(), 1,reinterpret_cast<VkDescriptorSet const *>(&imGuiDescriptorSet)), "Could not free ImGui texture");
    }
}