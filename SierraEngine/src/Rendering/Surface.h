//
// Created by Nikolay Kanchevski on 14.05.24.
//

#pragma once

namespace SierraEngine
{

    struct SurfaceCreateInfo
    {
        std::string_view title = "Sierra Engine Window";
        Sierra::SwapchainPresentationMode preferredPresentationMode = Sierra::SwapchainPresentationMode::VSync;

        const Sierra::PlatformContext& platformContext;
        const Sierra::Device& device;
    };

    class SIERRA_ENGINE_API Surface final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Surface(const SurfaceCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update() const;
        void Present(Sierra::CommandBuffer& commandBuffer) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] Sierra::Window& GetWindow() const noexcept { return *window; }
        [[nodiscard]] Sierra::Swapchain& GetSwapchain() const noexcept { return *swapchain; }

        /* --- COPY SEMANTICS --- */
        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;

        /* --- MOVE SEMANTICS --- */
        Surface(Surface&&) noexcept = default;
        Surface& operator=(Surface&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~Surface() noexcept = default;

    private:
        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;

    };

}
