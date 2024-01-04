//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "MetalSwapchain.h"

#include <QuartzCore/QuartzCore.h>

#include "MetalCommandBuffer.h"
#include "MetalImage.h"
#if SR_PLATFORM_macOS
    #include "../../../Core/Platform/macOS/CocoaWindow.h"
#elif SR_PLATFORM_iOS
    #include "../../../Core/Platform/iOS/UIKitWindow.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalSwapchain::MetalSwapchain(const MetalDevice &device, const SwapchainCreateInfo &createInfo)
        : Swapchain(createInfo), MetalResource(createInfo.name), device(device), window(*createInfo.window)
    {
        CAMetalLayer* caMetalLayer = nullptr;
        #if SR_PLATFORM_macOS
            SR_ERROR_IF(window.GetAPI() != PlatformAPI::Cocoa, "[Metal]: Cannot create Metal swapchain [{0}] for window [{1}], because its platform API does not match [PlatformAPI::Cocoa]!", GetName(), window.GetTitle());
            const CocoaWindow &cocoaWindow = static_cast<const CocoaWindow&>(window);

            // Retrieve Metal layer
            caMetalLayer = reinterpret_cast<CAMetalLayer*>(cocoaWindow.GetNSView().layer);
        #elif SR_PLATFORM_iOS
            SR_ERROR_IF(window.GetAPI() != PlatformAPI::UIKit, "[Metal]: Cannot create Metal swapchain [{0}] for window [{1}], because its platform API does not match [PlatformAPI::UIKit]!", GetName(), window.GetTitle());
            const UIKitWindow &uiKitWindow = static_cast<const UIKitWindow&>(window);

            // Retrieve Metal layer
            caMetalLayer = reinterpret_cast<CAMetalLayer*>(uiKitWindow.GetUIView().layer);
        #endif

        // Configure Metal layer
        [caMetalLayer setDevice: (__bridge id<MTLDevice>) device.GetMetalDevice()];
        switch (createInfo.preferredImageMemoryType) // These formats are guaranteed to be supported
        {
            case SwapchainImageMemoryType::UNorm8:      { [caMetalLayer setPixelFormat: MTLPixelFormatBGRA8Unorm ];      break; }
            case SwapchainImageMemoryType::SRGB8:       { [caMetalLayer setPixelFormat: MTLPixelFormatBGRA8Unorm_sRGB ]; break; }
            case SwapchainImageMemoryType::UNorm16:     { [caMetalLayer setPixelFormat: MTLPixelFormatRGBA16Float];      break; }
            default:                                    break;
        }
        [caMetalLayer setMaximumDrawableCount: CONCURRENT_FRAME_COUNT];
        [caMetalLayer setDrawsAsynchronously: YES];
        [caMetalLayer setDrawableSize: CGSizeMake(window.GetFramebufferSize().x, window.GetFramebufferSize().y)];
        #if SR_PLATFORM_macOS
            [caMetalLayer setDisplaySyncEnabled: createInfo.preferredPresentationMode == SwapchainPresentationMode::VSync]; // This is only present on macOS
        #endif
        metalLayer = (__bridge CA::MetalLayer*) caMetalLayer;

        // Create swapchain image (unnecessary on Metal, but needs to comply with Vulkan design)
        @autoreleasepool { metalDrawable = metalLayer->nextDrawable()->retain(); }
        swapchainImage = std::unique_ptr<MetalImage>(new MetalImage(device, MetalImage::SwapchainImageCreateInfo {
            .name = "Swapchain [" + GetName() + "]'s image",
            .texture = metalDrawable->texture(),
            .width = static_cast<uint32>(metalLayer->drawableSize().width),
            .height = static_cast<uint32>(metalLayer->drawableSize().height),
            .format = metalLayer->pixelFormat()
        }));

        // Create sync objects
        isFrameRenderedSemaphores = dispatch_semaphore_create(CONCURRENT_FRAME_COUNT);

        // Handle resizing
        createInfo.window->OnEvent<WindowResizeEvent>([this](const WindowResizeEvent &event)
        {
            // Resize Metal layer
            metalLayer->setDrawableSize(CGSizeMake(window.GetFramebufferSize().x, window.GetFramebufferSize().y));

            // Recreate swapchain images
            Recreate();

            // Dispatch resize event
            GetSwapchainResizeDispatcher().DispatchEvent(Vector2UInt(metalLayer->drawableSize().width, metalLayer->drawableSize().height));

            return false;
        });
    }

    /* --- POLLING METHODS --- */

    void MetalSwapchain::AcquireNextImage()
    {
        // Wait until GPU is done with the frame
        dispatch_semaphore_wait(isFrameRenderedSemaphores, DISPATCH_TIME_FOREVER);

        // Acquire next drawable
        metalDrawable->release();
        @autoreleasepool { metalDrawable = metalLayer->nextDrawable()->retain(); }

        // Update actual swapchain image's texture
        static_cast<MetalImage&>(*swapchainImage).texture = metalDrawable->texture();
    }

    void MetalSwapchain::SubmitCommandBufferAndPresent(std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot present swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        // Record presentation commands
        metalCommandBuffer.GetMetalCommandBuffer()->presentDrawable(metalDrawable);
        metalCommandBuffer.GetMetalCommandBuffer()->addCompletedHandler(^(MTL::CommandBuffer*) { dispatch_semaphore_signal(isFrameRenderedSemaphores); });

        // Submit command buffer
        metalCommandBuffer.GetMetalCommandBuffer()->commit();

        // Increment current frame
        currentFrame = (currentFrame + 1) % CONCURRENT_FRAME_COUNT;
    }

    /* --- DESTRUCTOR --- */

    MetalSwapchain::~MetalSwapchain()
    {
        metalLayer->release();
    }

    /* --- PRIVATE METHODS --- */

    void MetalSwapchain::Recreate()
    {
        // Recreate image with new dimensions
        swapchainImage = std::unique_ptr<MetalImage>(new MetalImage(this->device, MetalImage::SwapchainImageCreateInfo {
            .name = "Swapchain [" + GetName() + "]'s image",
            .width = static_cast<uint32>(metalLayer->drawableSize().width),
            .height = static_cast<uint32>(metalLayer->drawableSize().height),
            .format = metalLayer->pixelFormat()
        }));
    }

}
