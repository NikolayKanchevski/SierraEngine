//
// Created by Nikolay Kanchevski on 14.05.24.
//

#include "EditorSurface.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorSurface::EditorSurface(const SurfaceCreateInfo &createInfo)
        : Surface(createInfo), renderingContext(createInfo.renderingContext)
    {
        window = createInfo.windowManager.CreateWindow({
            .title = "Sierra Engine Editor",
            .resizable = false,
            .maximize = false,
            .hide = true
        });

        swapchain = createInfo.renderingContext.CreateSwapchain({
            .name = "Editor Surface Swapchain",
            .window = *window,
            .preferredPresentationMode = Sierra::SwapchainPresentationMode::VSync,
            .preferredBuffering = Sierra::SwapchainBuffering::TripleBuffering
        });

        CreateRenderTargets();
    }

    /* --- POLLING METHODS --- */

    void EditorSurface::Update() const
    {
        swapchain->AcquireNextImage();
    }

    void EditorSurface::Present(Sierra::CommandBuffer &commandBuffer) const
    {
        swapchain->Present(commandBuffer);

        if (window->IsHidden()) window->Show();
        window->Update();
    }

    /* --- PRIVATE METHODS --- */

    void EditorSurface::CreateRenderTargets()
    {
        constexpr Sierra::ImageUsage IMAGE_USAGE = Sierra::ImageUsage::ColorAttachment | Sierra::ImageUsage::Sample;
        const std::optional<Sierra::ImageFormat> format = renderingContext.GetDevice().GetSupportedImageFormat(Sierra::ImageFormat::R8G8B8A8_UNorm, IMAGE_USAGE);
        APP_ERROR_IF(!format.has_value(), "[Sierra Engine Editor]: Cannot create surface, as no matching image format for [ImageFormat::R8G8B8A8_UNorm] is supported by the device of rendering context [{0}]!", renderingContext.GetName());

        renderTargets.resize(swapchain->GetConcurrentFrameCount());
        for (size i = 0; i < renderTargets.size(); i++)
        {
            renderTargets[i] = renderingContext.CreateImage({
                .name = fmt::format("Staging Image of Editor Surface [{0}]", i),
                .width = swapchain->GetWidth(),
                .height = swapchain->GetHeight(),
                .format = format.value(),
                .usage = IMAGE_USAGE,
                .memoryLocation = Sierra::ImageMemoryLocation::GPU
            });
        }
    }


}