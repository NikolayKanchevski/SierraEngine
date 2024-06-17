//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "MetalSwapchain.h"

#include <QuartzCore/QuartzCore.h>

#include "MetalImage.h"
#include "MetalCommandBuffer.h"
#if SR_PLATFORM_macOS
    #include "../../../Core/Platform/macOS/CocoaWindow.h"
#elif SR_PLATFORM_iOS
    #include "../../../Core/Platform/iOS/UIKitWindow.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalSwapchain::MetalSwapchain(const MetalDevice &device, const SwapchainCreateInfo &createInfo)
        : Swapchain(createInfo), MetalResource(createInfo.name), device(device), window(createInfo.window)
    {
        #if SR_PLATFORM_macOS
            SR_ERROR_IF(window.GetAPI() != PlatformAPI::Cocoa, "[Metal]: Cannot create Metal swapchain [{0}] for window [{1}], because its platform API does not match [PlatformAPI::Cocoa]!", GetName(), window.GetTitle());
            const CocoaWindow &cocoaWindow = static_cast<const CocoaWindow&>(window);
            metalLayer = reinterpret_cast<CAMetalLayer*>(cocoaWindow.GetNSView().layer);
        #elif SR_PLATFORM_iOS
            SR_ERROR_IF(window.GetAPI() != PlatformAPI::UIKit, "[Metal]: Cannot create Metal swapchain [{0}] for window [{1}], because its platform API does not match [PlatformAPI::UIKit]!", GetName(), window.GetTitle());
            const UIKitWindow &uiKitWindow = static_cast<const UIKitWindow&>(window);
            metalLayer = reinterpret_cast<CAMetalLayer*>(uiKitWindow.GetUIView().layer);
        #endif

        // Determine concurrent frame count
        switch (createInfo.preferredBuffering)
        {
            case SwapchainBuffering::DoubleBuffering:       { concurrentFrameCount = 2; break; }
            case SwapchainBuffering::TripleBuffering:       { concurrentFrameCount = 3; break; }
        }

        // Configure Metal layer
        [metalLayer setDevice: device.GetMetalDevice()];
        [metalLayer setMaximumDrawableCount: concurrentFrameCount];
        [metalLayer setDrawsAsynchronously: YES];
        [metalLayer setDrawableSize: CGSizeMake(window.GetFramebufferWidth(), window.GetFramebufferHeight())];
        switch (createInfo.preferredImageMemoryType) // These formats are guaranteed to be supported
        {
            case SwapchainImageMemoryType::UNorm8:      { [metalLayer setPixelFormat: MTLPixelFormatBGRA8Unorm ];      break; }
            case SwapchainImageMemoryType::SRGB8:       { [metalLayer setPixelFormat: MTLPixelFormatBGRA8Unorm_sRGB ]; break; }
            case SwapchainImageMemoryType::UNorm16:     { [metalLayer setPixelFormat: MTLPixelFormatRGBA16Float];      break; }
        }
        #if SR_PLATFORM_macOS
            [metalLayer setDisplaySyncEnabled:createInfo.preferredPresentationMode == SwapchainPresentationMode::VSync]; // This is only present on macOS
        #endif
        metalDrawable = [metalLayer nextDrawable];

        // Create swapchain image (unnecessary on Metal, but needs to comply with Vulkan design)
        swapchainImage = std::unique_ptr<MetalImage>(new MetalImage(device, MetalImage::SwapchainImageCreateInfo {
            .name = "Image of swapchain [" + std::string(GetName()) + "]",
            .texture = metalDrawable.texture,
            .width = static_cast<uint32>(metalLayer.drawableSize.width),
            .height = static_cast<uint32>(metalLayer.drawableSize.height),
            .format = metalLayer.pixelFormat
        }));

        // Create sync objects
        isFrameRenderedSemaphores = dispatch_semaphore_create(concurrentFrameCount);

        // Handle resizing
        createInfo.window.OnEvent<WindowResizeEvent>([this](const WindowResizeEvent&) -> bool
        {
            // Resize Metal layer
            [metalLayer setDrawableSize: CGSizeMake(window.GetFramebufferWidth(), window.GetFramebufferHeight())];

            // Recreate swapchain images
            Recreate();

            // Dispatch resize event
            GetSwapchainResizeDispatcher().DispatchEvent(window.GetFramebufferWidth(), window.GetFramebufferHeight(), GetScaling());
            return false;
        });
    }

    /* --- POLLING METHODS --- */

    void MetalSwapchain::AcquireNextImage()
    {
        // Wait until current frame has been presented
        dispatch_semaphore_wait(isFrameRenderedSemaphores, DISPATCH_TIME_FOREVER);

        // Acquire next drawable
        metalDrawable = [metalLayer nextDrawable];
        
        // Update image
        swapchainImage->texture = metalDrawable.texture;
    }

    void MetalSwapchain::Present(CommandBuffer &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot present swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer.GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        // Record presentation commands to a new command buffer (width a dependency to passed one)
        const id<MTLCommandBuffer> presentationCommandBuffer = [device.GetCommandQueue() commandBuffer];
        device.SetResourceName(presentationCommandBuffer, "Presentation command buffer of swapchain [" + std::string(GetName()) + "]");
        [presentationCommandBuffer encodeWaitForEvent: device.GetSharedSignalSemaphore() value: metalCommandBuffer.GetCompletionSignalValue()];
        [presentationCommandBuffer presentDrawable: metalDrawable];
        [presentationCommandBuffer addCompletedHandler: ^(id<MTLCommandBuffer> executedCommandBuffer)
        {
            dispatch_semaphore_signal(isFrameRenderedSemaphores);
        }];
        [presentationCommandBuffer commit];

        // Increment current frame
        currentFrame = (currentFrame + 1) % concurrentFrameCount;
    }

    /* --- PRIVATE METHODS --- */

    void MetalSwapchain::Recreate()
    {
        // Recreate image with new dimensions
        swapchainImage = std::unique_ptr<MetalImage>(new MetalImage(this->device, MetalImage::SwapchainImageCreateInfo {
            .name = "Image of swapchain [" + std::string(GetName()) + "]",
            .width = static_cast<uint32>(metalLayer.drawableSize.width),
            .height = static_cast<uint32>(metalLayer.drawableSize.height),
            .format = metalLayer.pixelFormat
        }));
    }

}
