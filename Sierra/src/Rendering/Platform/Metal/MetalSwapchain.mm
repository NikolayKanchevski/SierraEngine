//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "MetalSwapchain.h"

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
        [caMetalLayer setPixelFormat: MTLPixelFormatBGRA8Unorm_sRGB];
        [caMetalLayer setDrawableSize: CGSizeMake(window.GetFramebufferSize().x, window.GetFramebufferSize().y)];
        [caMetalLayer setMaximumDrawableCount: CONCURRENT_FRAME_COUNT];
        [caMetalLayer setDrawsAsynchronously: YES];

        // Bridge native CALayer to C++ wrapper
        metalLayer = (__bridge CA::MetalLayer*) caMetalLayer;

        // Create render pass
        renderPass = MTL::RenderPassDescriptor::alloc()->init();
        MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPass->colorAttachments()->object(0);
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setClearColor(MTL::ClearColor(1.0f, 0.0f, 0.0f, 1.0f));
        colorAttachment->setStoreAction(MTL::StoreActionStore);

        // Create sync objects
        isFrameRenderedSemaphores = dispatch_semaphore_create(CONCURRENT_FRAME_COUNT);
    }

    /* --- POLLING METHODS --- */

    void MetalSwapchain::Begin(const std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        // Wait until GPU is done with the frame
        dispatch_semaphore_wait(isFrameRenderedSemaphores, DISPATCH_TIME_FOREVER);

        // Acquire next drawable and set to use that
        metalDrawable = metalLayer->nextDrawable();
        renderPass->colorAttachments()->object(0)->setTexture(metalDrawable->texture());

        // Begin render pass
        currentRenderCommandEncoder = metalCommandBuffer.GetMetalCommandBuffer()->renderCommandEncoder(renderPass);
    }

    void MetalSwapchain::End(const std::unique_ptr<CommandBuffer> &commandBuffer)
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end swapchain [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());

        // End render pass
        currentRenderCommandEncoder->endEncoding();
    }

    void MetalSwapchain::SubmitCommandBufferAndPresent(const std::unique_ptr<CommandBuffer> &commandBuffer)
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

    void MetalSwapchain::Destroy()
    {
        metalLayer->release();
        renderPass->release();
    }

}
