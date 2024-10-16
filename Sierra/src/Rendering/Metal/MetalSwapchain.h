//
// Created by Nikolay Kanchevski on 11.12.23.
//

#pragma once

#include "../Swapchain.h"
#include "MetalResource.h"

#include "MetalImage.h"
#include "MetalDevice.h"
#include "../../Windowing/Window.h"

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
        [[nodiscard]] std::string_view GetName() const noexcept override { return name; }

        [[nodiscard]] uint32 GetCurrentFrameIndex() const noexcept override { return currentFrame; }
        [[nodiscard]] uint32 GetCurrentImageIndex() const noexcept override { return currentFrame; }
        [[nodiscard]] uint32 GetConcurrentFrameCount() const noexcept override { return concurrentFrameCount; }

        [[nodiscard]] uint32 GetScaling() const noexcept override { return swapchainImage->GetWidth() / window.GetWidth(); }
        [[nodiscard]] const Image& GetImage(const uint32 frameIndex) const override { SR_THROW_IF(frameIndex >= concurrentFrameCount, ValueOutOfRangeError(SR_FORMAT("Cannot get image [{0}] of swapchain [{1}]! Use Swapchain::GetConcurrentFrameCount() to query count", frameIndex, name), frameIndex, 0U, GetConcurrentFrameCount() - 1)); return *swapchainImage; }

        /* --- COPY SEMANTICS --- */
        MetalSwapchain(const MetalSwapchain&) = delete;
        MetalSwapchain& operator=(const MetalSwapchain&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalSwapchain(MetalSwapchain&&) = delete;
        MetalSwapchain& operator=(MetalSwapchain&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalSwapchain() noexcept override;

    private:
        const MetalDevice& device;
        const std::string name;

        Window& window;

        #if !defined(__OBJC__)
          using CAMetalLayer = void;
          using CAMetalDrawable = void;
        #endif
        CAMetalLayer* metalLayer = nil;
        id<CAMetalDrawable> metalDrawable = nil;

        std::unique_ptr<MetalImage> swapchainImage = nullptr;
        EventSubscriptionID windowResizeEventListenerID;
        dispatch_semaphore_t isFrameRenderedSemaphores = nil;

        uint32 currentFrame = 0;
        uint32 concurrentFrameCount = 0;
        void Recreate();

    };

}
