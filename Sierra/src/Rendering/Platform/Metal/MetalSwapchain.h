//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../Swapchain.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "../../../Core/Window.h"

namespace Sierra
{

    class SIERRA_API MetalSwapchain : public Swapchain, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalSwapchain(const MetalDevice &device, const SwapchainCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void AcquireNextImage() override;
        void SubmitCommandBufferAndPresent(std::unique_ptr<CommandBuffer> &commandBuffer) override;

        /* -- GETTER METHODS --- */
        [[nodiscard]] inline uint32 GetCurrentFrame() const override { return currentFrame; }
        [[nodiscard]] inline uint32 GetConcurrentFrameCount() const override { return CONCURRENT_FRAME_COUNT; }

        [[nodiscard]] inline const std::unique_ptr<Image>& GetCurrentImage() const override { return swapchainImage; };
        [[nodiscard]] inline const std::unique_ptr<Image>& GetImage(const uint32 frameIndex) const override { SR_ERROR_IF(frameIndex >= CONCURRENT_FRAME_COUNT, "[Metal]: Cannot return image with an index [{0}] of swapchain [{1}], as index is out of bounds! Use Swapchain::GetConcurrentFrameCount() to query image count.", frameIndex, GetName()); return swapchainImage; };

        /* --- DESTRUCTOR --- */
        ~MetalSwapchain();

    private:
        const MetalDevice &device;
        const Window &window;

        CA::MetalLayer* metalLayer = nullptr;
        CA::MetalDrawable* metalDrawable = nullptr;

        std::unique_ptr<Image> swapchainImage;
        dispatch_semaphore_t isFrameRenderedSemaphores = nullptr;

        uint32 currentFrame = 0;
        constexpr static NS::UInteger CONCURRENT_FRAME_COUNT = 3;
        void Recreate();

    };

}
