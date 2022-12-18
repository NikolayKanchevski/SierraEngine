//
// Created by Nikolay Kanchevski on 10.12.22.
//

#pragma once

#include "Sampler.h"
#include "RenderPass.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    typedef enum AntiAliasingType
    {
        ANTI_ALIASING_NONE = 0x00000001,
        ANTI_ALIASING_MSAAx2 = 0x00000002,
        ANTI_ALIASING_MSAAx4 = 0x00000004,
        ANTI_ALIASING_MSAAx8 = 0x00000008,
        ANTI_ALIASING_MSAAx16 = 0x00000010,
        ANTI_ALIASING_MSAAx32 = 0x00000020,
        ANTI_ALIASING_MSAAx64 = 0x00000020
    } AntiAliasingType;

    typedef enum AttachmentType
    {
        ATTACHMENT_COLOR = 0x00000001,
        ATTACHMENT_DEPTH = 0x00000002
    } AttachmentType;

    struct OffscreenRendererCreateInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t maxConcurrentFrames = 0;

        AttachmentType attachmentTypes;
        AntiAliasingType antiAliasingType = ANTI_ALIASING_NONE;
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

        AttachmentType attachmentTypes;
        AntiAliasingType antiAliasingType;

        bool alreadyCreated = false;

        std::vector<std::unique_ptr<Image>> colorImages;
        std::unique_ptr<Image> depthImage;
        std::unique_ptr<Image> msaaImage;

        std::unique_ptr<Sampler> sampler;

        std::unique_ptr<RenderPass> renderPass;
        std::vector<std::unique_ptr<Framebuffer>> framebuffers;

        void CreateImages();
        void CreateRenderPass();
        void CreateFramebuffers();

        inline bool HasColorAttachment() const { return attachmentTypes & ATTACHMENT_COLOR; }
        inline bool HasDepthAttachment() const { return attachmentTypes & ATTACHMENT_DEPTH; }
        inline bool HasMSAAEnabled() const { return antiAliasingType >= ANTI_ALIASING_MSAAx2 && antiAliasingType <= ANTI_ALIASING_MSAAx64; }
    };

}
