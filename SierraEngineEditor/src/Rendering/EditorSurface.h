//
// Created by Nikolay Kanchevski on 14.05.24.
//

#pragma once

namespace SierraEngine
{

    class EditorSurface final : public Surface
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorSurface(const SurfaceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update() const override;
        void Present(Sierra::CommandBuffer &commandBuffer) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Image& GetCurrentRenderTarget() const { return *renderTargets[swapchain->GetCurrentImageIndex()]; }
        [[nodiscard]] const Sierra::Image& GetCurrentSwapchainImage() const override { return swapchain->GetCurrentImage(); }

        [[nodiscard]] Sierra::Window& GetWindow() const override { return *window; }
        [[nodiscard]] Sierra::Swapchain& GetSwapchain() const override { return *swapchain; }

        /* --- DESTRUCTOR --- */
        ~EditorSurface() override = default;

    private:
        const Sierra::RenderingContext &renderingContext;
        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;

        std::vector<std::unique_ptr<Sierra::Image>> renderTargets = { };
        void CreateRenderTargets();

    };

}
