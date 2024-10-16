//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "MetalSwapchain.h"

#include <QuartzCore/QuartzCore.h>

#include "MetalImage.h"
#include "MetalCommandBuffer.h"
#if SR_PLATFORM_macOS
    #include "../../Windowing/macOS/CocoaWindow.h"
#elif SR_PLATFORM_iOS
    #include "../../Windowing/iOS/UIKitWindow.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalSwapchain::MetalSwapchain(const MetalDevice& device, const SwapchainCreateInfo& createInfo)
        : Swapchain(createInfo), device(device), window(createInfo.window), name(createInfo.name)
    {
        #if SR_PLATFORM_macOS
            SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Cocoa, UnexpectedTypeError(SR_FORMAT("Cannot create swapchain [{0}] using window [{1}], as its platform backend differs from [PlatformBackendType::Cocoa]", name, window.GetTitle())));
            const CocoaWindow& cocoaWindow = static_cast<const CocoaWindow&>(window);
            metalLayer = reinterpret_cast<CAMetalLayer*>(cocoaWindow.GetNSView().layer);
        #elif SR_PLATFORM_iOS
            SR_THROW_IF(window.GetBackendType() != WindowingBackendType::UIKit, UnexpectedTypeError(SR_FORMAT("Cannot create swapchain [{0}] using window [{1}], as its platform backend differs from [PlatformBackendType::UIKit]", name, window.GetTitle())));
            const UIKitWindow& uiKitWindow = static_cast<const UIKitWindow&>(window);
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
            .name = SR_FORMAT("Image of swapchain [{0}]", name),
            .texture = metalDrawable.texture,
            .width = static_cast<uint32>(metalLayer.drawableSize.width),
            .height = static_cast<uint32>(metalLayer.drawableSize.height),
            .format = metalLayer.pixelFormat
        }));

        // Create sync objects
        isFrameRenderedSemaphores = dispatch_semaphore_create(concurrentFrameCount);

        // Handle resizing
        windowResizeEventListenerID = createInfo.window.AddEventListener<WindowResizeEvent>([this](const WindowResizeEvent&) -> bool
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
        SR_THROW_IF(window.IsClosed(), InvalidOperationError(SR_FORMAT("Cannot acquire next image of swapchain [{0}], as its corresponding window [{1}] has been closed", name, window.GetTitle())));

        // Wait until current frame has been presented
        dispatch_semaphore_wait(isFrameRenderedSemaphores, DISPATCH_TIME_FOREVER);

        // Acquire next drawable
        metalDrawable = [metalLayer nextDrawable];
        
        // Update image
        swapchainImage->texture = metalDrawable.texture;
    }

    void MetalSwapchain::Present(CommandBuffer& commandBuffer)
    {
        SR_THROW_IF(commandBuffer.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot present swapchain [{0}] using command buffer [{1}], as its backend type differs from [RenderingBackendType::Metal]", name, commandBuffer.GetName())));
        const MetalCommandBuffer& metalCommandBuffer = static_cast<const MetalCommandBuffer&>(commandBuffer);

        SR_THROW_IF(window.IsClosed(), InvalidOperationError(SR_FORMAT("Cannot present swapchain [{0}], as its corresponding window [{1}] has been closed", name, window.GetTitle())));

        // Record presentation commands to a new command buffer (width a dependency to passed one)
        const id<MTLCommandBuffer> presentationCommandBuffer = [metalCommandBuffer.GetQueue().GetMetalCommandQueue() commandBuffer];
        device.SetResourceName(presentationCommandBuffer, SR_FORMAT("Presentation command buffer of swapchain [{0}]", name));
        [presentationCommandBuffer encodeWaitForEvent: device.GetSemaphore() value: metalCommandBuffer.GetCompletionSemaphoreSignalValue()];
        [presentationCommandBuffer presentDrawable: metalDrawable];
        [presentationCommandBuffer addCompletedHandler: ^(id<MTLCommandBuffer> executedCommandBuffer)
        {
            dispatch_semaphore_signal(isFrameRenderedSemaphores);
        }];
        [presentationCommandBuffer commit];

        // Increment current frame
        currentFrame = (currentFrame + 1) % concurrentFrameCount;
    }

    /* --- DESTRUCTOR --- */

    MetalSwapchain::~MetalSwapchain() noexcept
    {
        window.RemoveEventListener<WindowResizeEvent>(windowResizeEventListenerID);
    }

    /* --- PRIVATE METHODS --- */

    void MetalSwapchain::Recreate()
    {
        // Recreate image with new dimensions
        swapchainImage = std::unique_ptr<MetalImage>(new MetalImage(device, MetalImage::SwapchainImageCreateInfo {
            .name = SR_FORMAT("Image of swapchain [{0}]", name),
            .width = static_cast<uint32>(metalLayer.drawableSize.width),
            .height = static_cast<uint32>(metalLayer.drawableSize.height),
            .format = metalLayer.pixelFormat
        }));
    }

}
