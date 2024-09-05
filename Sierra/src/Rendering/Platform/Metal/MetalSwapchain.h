//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../../Swapchain.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "../../../Core/Window.h"
#include "MetalImage.h"

namespace Sierra
{

    class SIERRA_API MetalSwapchain final : public Swapchain, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalSwapchain(const MetalDevice& device, const SwapchainCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void AcquireNextImage() override;
        void Present(CommandBuffer& commandBuffer) override;

        /* -- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const override;

        [[nodiscard]] uint32 GetCurrentFrameIndex() const override { return currentFrame; }
        [[nodiscard]] uint32 GetCurrentImageIndex() const override { return currentFrame; }
        [[nodiscard]] uint32 GetConcurrentFrameCount() const override { return concurrentFrameCount; }

        [[nodiscard]] uint32 GetScaling() const override { return swapchainImage->GetWidth() / window.GetWidth(); }
        [[nodiscard]] const Image& GetImage(const uint32 frameIndex) const override { SR_ERROR_IF(frameIndex >= concurrentFrameCount, "[Metal]: Cannot return image with an index [{0}] of swapchain [{1}], as index is out of bounds! Use Swapchain::GetConcurrentFrameCount() to query image count.", frameIndex, GetName()); return *swapchainImage; }

        /* --- DESTRUCTOR --- */
        ~MetalSwapchain() override;

    private:
        const MetalDevice& device;
        Window& window;

        std::string name;

        #if !defined(__OBJC__)
          using CAMetalLayer = void;
          using CAMetalDrawable = void;
        #endif
        CAMetalLayer* metalLayer = nil;
        id<CAMetalDrawable> metalDrawable = nil;

        std::unique_ptr<MetalImage> swapchainImage;
        EventSubscriptionID windowResizeEventListenerID;
        dispatch_semaphore_t isFrameRenderedSemaphores = nil;

        uint32 currentFrame = 0;
        uint32 concurrentFrameCount = 0;
        void Recreate();

    };

}
