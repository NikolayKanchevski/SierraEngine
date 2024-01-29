//
// Created by Nikolay Kanchevski on 28.01.24.
//

#pragma once

#include "../Core/Window.h"
#include "RenderingContext.h"

#include "Swapchain.h"

namespace Sierra
{

    struct RendererCreateInfo
    {
        std::unique_ptr<Window> &window;
        const std::unique_ptr<RenderingContext> &renderingContext;
        SwapchainPresentationMode preferredPresentationMode = SwapchainPresentationMode::VSync;
    };

    class SIERRA_API Renderer
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Render() = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const std::unique_ptr<Image>& GetRenderTarget() const = 0;

        /* --- OPERATORS --- */
        Renderer(const Renderer&) = delete;
        Renderer &operator=(const Renderer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~Renderer() = default;

    protected:
        explicit Renderer(const RendererCreateInfo &createInfo);

        [[nodiscard]] inline std::unique_ptr<Window>& GetWindow() { return window; }
        [[nodiscard]] inline std::unique_ptr<Swapchain>& GetSwapchain() { return swapchain; }

    private:
        std::unique_ptr<Window> &window;
        std::unique_ptr<Swapchain> swapchain = nullptr;

    };

}
