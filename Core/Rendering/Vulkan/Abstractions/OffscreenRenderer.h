//
// Created by Nikolay Kanchevski on 10.12.22.
//

#pragma once

#include "Sampler.h"
#include "RenderPass.h"

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    typedef uint AttachmentType;
    typedef enum _AttachmentType
    {
        ATTACHMENT_COLOR = 0x00000001,
        ATTACHMENT_DEPTH = 0x00000002
    } _AttachmentType;

    struct OffscreenRendererCreateInfo
    {
        uint width = 0;
        uint height = 0;
        uint maxConcurrentFrames = 0;

        AttachmentType attachmentTypes;
        Sampling msaaSampling = Sampling::MSAAx1;
    };

    class OffscreenRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        OffscreenRenderer(const OffscreenRendererCreateInfo &createInfo);
        static UniquePtr<OffscreenRenderer> Create(OffscreenRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Begin(const VkCommandBuffer &givenCommandBuffer, uint frameIndex) const;
        void End(const VkCommandBuffer &givenCommandBuffer) const;

        /* --- SETTER METHODS --- */
        void Resize(uint newWidth, uint newHeight);
        void SetMultisampling(const Sampling newMultisampling);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint GetWidth() const { return this->width; }
        [[nodiscard]] inline uint GetHeight() const { return this->height; }

        [[nodiscard]] inline UniquePtr<Image>& GetColorImage(const uint colorImageIndex) { return this->colorImages[colorImageIndex]; }
        [[nodiscard]] inline UniquePtr<Image>& GetDepthImage() { return this->depthImage; }
        [[nodiscard]] inline UniquePtr<Sampler>& GetSampler() { return this->sampler; }
        [[nodiscard]] inline UniquePtr<RenderPass>& GetRenderPass() { return this->renderPass; }

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(OffscreenRenderer);

    private:
        uint width, height, maxConcurrentFrames;

        AttachmentType attachmentTypes;
        Sampling msaaSampling = Sampling::MSAAx2;

        bool alreadyCreated = false;

        std::vector<UniquePtr<Image>> colorImages;
        UniquePtr<Image> depthImage;
        UniquePtr<Image> msaaImage;

        UniquePtr<Sampler> sampler;

        UniquePtr<RenderPass> renderPass;
        std::vector<UniquePtr<Framebuffer>> framebuffers;

        void CreateImages();
        void CreateRenderPass();
        void CreateFramebuffers();

        inline bool HasColorAttachment() const { return attachmentTypes & ATTACHMENT_COLOR; }
        inline bool HasDepthAttachment() const { return attachmentTypes & ATTACHMENT_DEPTH; }
        inline bool HasMSAAEnabled() const { return msaaSampling > Sampling::MSAAx1; }
    };

}
