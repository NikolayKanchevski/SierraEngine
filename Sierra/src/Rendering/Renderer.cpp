//
// Created by Nikolay Kanchevski on 28.01.24.
//

#include "Renderer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Renderer::Renderer(const RendererCreateInfo &createInfo)
        : window(createInfo.window)
    {
        swapchain = createInfo.renderingContext->CreateSwapchain({
            .name = "Swapchain of window [" + window->GetTitle() + "]",
            .window = createInfo.window,
            .preferredPresentationMode = createInfo.preferredPresentationMode,
            .preferredImageMemoryType = SwapchainImageMemoryType::UNorm8
        });
    }

}