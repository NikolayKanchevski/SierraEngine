//
// Created by Nikolay Kanchevski on 14.05.24.
//

#include "Surface.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Surface::Surface(const SurfaceCreateInfo& createInfo)
    {
        window = createInfo.platformContext.CreateWindow({
            .title = createInfo.title,
            .resizable = false,
            .maximize = false,
            .hide = true
        });

        swapchain = createInfo.device.CreateSwapchain({
            .name = SR_FORMAT("Swapchain of window [{0}]", createInfo.title),
            .window = *window,
            .preferredPresentationMode = Sierra::SwapchainPresentationMode::VSync,
            .preferredBuffering = Sierra::SwapchainBuffering::TripleBuffering
        });
    }

    /* --- POLLING METHODS --- */

    void Surface::Update() const
    {
        swapchain->AcquireNextImage();
    }

    void Surface::Present(Sierra::CommandBuffer& commandBuffer) const
    {
        swapchain->Present(commandBuffer);

        if (window->IsHidden()) window->Show();
        window->Update();
    }

}