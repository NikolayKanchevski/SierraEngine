//
// Created by Nikolay Kanchevski on 9.09.23.
//

#pragma once

#include "RenderingResource.h"

#include "Device.h"
#include "Buffer.h"
#include "Image.h"
#include "RenderPass.h"
#include "Swapchain.h"
#include "CommandBuffer.h"

namespace Sierra
{

    struct RenderingContextCreateInfo
    {
        const std::string &name = "Rendering Context";
        GraphicsAPI graphicsAPI = GraphicsAPI::Auto;
    };

    class SIERRA_API RenderingContext : public virtual RenderingResource
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Buffer> CreateBuffer(const BufferCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Image> CreateImage(const ImageCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<RenderPass> CreateRenderPass(const RenderPassCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<Swapchain> CreateSwapchain(const SwapchainCreateInfo &createInfo) const = 0;
        [[nodiscard]] virtual std::unique_ptr<CommandBuffer> CreateCommandBuffer(const CommandBufferCreateInfo &createInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const Device& GetDevice() const = 0;

    protected:
        explicit RenderingContext(const RenderingContextCreateInfo &createInfo);

    private:
        friend class Application;
        static std::unique_ptr<RenderingContext> Create(const RenderingContextCreateInfo &createInfo);

        void Destroy() override;

    };

}
