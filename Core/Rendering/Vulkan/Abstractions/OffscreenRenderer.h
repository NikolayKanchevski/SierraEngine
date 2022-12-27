//
// Created by Nikolay Kanchevski on 10.12.22.
//

#pragma once

#include "Sampler.h"
#include "RenderPass.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    typedef uint32_t AttachmentType;
    typedef enum _AttachmentType
    {
        ATTACHMENT_COLOR = 0x00000001,
        ATTACHMENT_DEPTH = 0x00000002
    } _AttachmentType;

    struct OffscreenRendererCreateInfo
    {
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t maxConcurrentFrames = 0;

        AttachmentType attachmentTypes;
        Sampling msaaSampling = MSAAx1;
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
        void SetMultisampling(const Sampling newMultisampling);

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
        Sampling msaaSampling;

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
        inline bool HasMSAAEnabled() const { return msaaSampling > MSAAx1; }
    };

}
