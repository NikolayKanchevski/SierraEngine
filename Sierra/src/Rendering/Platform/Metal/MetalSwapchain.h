//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../Swapchain.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "../../../Core/Window.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void CAMetalLayer;
        typedef void MetalView;
    }
#else
    #include <QuartzCore/QuartzCore.h>
    @class MetalView;
#endif

namespace Sierra
{

    class SIERRA_API MetalSwapchain : public Swapchain, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalSwapchain(const MetalDevice &device, const SwapchainCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Begin(const std::unique_ptr<CommandBuffer> &commandBuffer) override;
        void End(const std::unique_ptr<CommandBuffer> &commandBuffer) override;
        void SubmitCommandBufferAndPresent(const std::unique_ptr<CommandBuffer> &commandBuffer) override;

        /* -- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetCurrentFrame() const override { return currentFrame; }
        [[nodiscard]] inline uint32 GetConcurrentFrameCount() const override { return CONCURRENT_FRAME_COUNT; }

        [[nodiscard]] inline uint32 GetWidth() const override { return static_cast<uint32>(metalLayer->drawableSize().width); }
        [[nodiscard]] inline uint32 GetHeight() const override { return static_cast<uint32>(metalLayer->drawableSize().height); }

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        const MetalDevice &device;
        const Window &window;

        CA::MetalLayer* metalLayer = nullptr;
        CA::MetalDrawable* metalDrawable = nullptr;

        MTL::RenderPassDescriptor* renderPass = nullptr;
        MTL::RenderCommandEncoder* currentRenderCommandEncoder = nullptr;
        dispatch_semaphore_t isFrameRenderedSemaphores = nullptr;

        uint32 currentFrame = 0;
        constexpr static NS::UInteger CONCURRENT_FRAME_COUNT = 3;

    };

}
