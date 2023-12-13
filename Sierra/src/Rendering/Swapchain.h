//
// Created by Nikolay Kanchevski on 6.12.23.
//

#pragma once

#include "RenderingResource.h"
#include "../Core/Window.h"
#include "CommandBuffer.h"

namespace Sierra
{

    enum class SwapchainPresentationMode
    {
        Immediate,
        VSync
    };

    struct SwapchainCreateInfo
    {
        const std::string &name = "Swapchain";
        std::unique_ptr<Window> &window;
        SwapchainPresentationMode preferredPresentationMode = SwapchainPresentationMode::VSync;
    };

    class SIERRA_API Swapchain : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Begin(const std::unique_ptr<CommandBuffer> &commandBuffer) = 0;
        virtual void End(const std::unique_ptr<CommandBuffer> &commandBuffer) = 0;
        virtual void SubmitCommandBufferAndPresent(const std::unique_ptr<CommandBuffer> &commandBuffer) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint32 GetCurrentFrame() const = 0;
        [[nodiscard]] virtual uint32 GetConcurrentFrameCount() const = 0;

        [[nodiscard]] virtual uint32 GetWidth() const = 0;
        [[nodiscard]] virtual uint32 GetHeight() const = 0;

        /* --- OPERATORS --- */
        Swapchain(const Swapchain&) = delete;
        Swapchain& operator=(const Swapchain&) = delete;

    protected:
        explicit Swapchain(const SwapchainCreateInfo &createInfo);

    };

}
