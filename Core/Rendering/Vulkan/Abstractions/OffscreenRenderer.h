//
// Created by Nikolay Kanchevski on 10.12.22.
//

#pragma once

#include <cstdint>

#include "Sampler.h"
#include "RenderPass.h"
#include "Framebuffer.h"
#include "RenderPass.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    struct OffscreenRendererCreateInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t maxConcurrentFrames = 0;

        VkSampleCountFlagBits msaaSampling = VK_SAMPLE_COUNT_1_BIT;
        bool createDepthAttachment = false;
    };

    class OffscreenRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        OffscreenRenderer(const OffscreenRendererCreateInfo &createInfo);
        static std::unique_ptr<OffscreenRenderer> Create(OffscreenRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Begin(const VkCommandBuffer &givenCommandBuffer, uint32_t frameIndex) const;
        void End(const VkCommandBuffer &givenCommandBuffer) const;

        /* --- SETTER METHODS --- */
        void Resize(uint32_t newWidth, uint32_t newHeight);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint32_t GetWidth() const { return this->width; }
        [[nodiscard]] inline uint32_t GetHeight() const { return this->height; }

        [[nodiscard]] inline std::unique_ptr<Image>& GetColorImage(const uint32_t colorImageIndex) { return this->colorImages[colorImageIndex]; }
        [[nodiscard]] inline std::unique_ptr<Image>& GetDepthImage() { return this->depthImage; }
        [[nodiscard]] inline std::unique_ptr<Sampler>& GetSampler() { return this->sampler; }
        [[nodiscard]] inline std::unique_ptr<RenderPass>& GetRenderPass() { return this->renderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        OffscreenRenderer(const OffscreenRenderer &) = delete;
        OffscreenRenderer &operator=(const OffscreenRenderer &) = delete;

    private:
        uint32_t width, height, maxConcurrentFrames;

        bool alreadyCreated = false;
        bool hasDepthAttachment = false;
        VkSampleCountFlagBits msaaSampleCount;

        std::vector<std::unique_ptr<Image>> colorImages;
        std::unique_ptr<Image> depthImage;
        std::unique_ptr<Image> msaaImage;

        std::unique_ptr<Sampler> sampler;

        std::unique_ptr<RenderPass> renderPass;
        std::vector<std::unique_ptr<Framebuffer>> framebuffers;

        void CreateImages();
        void CreateRenderPass();
        void CreateFramebuffers();

        inline bool IsMSAAEnabled() const { return msaaSampleCount > VK_SAMPLE_COUNT_1_BIT; }
    };

}
