//
// Created by Nikolay Kanchevski on 18.06.24.
//

#pragma once

namespace SierraEngine
{

    struct SurfaceCreateInfo
    {
        std::string_view windowTitle = "Sierra Engine Window";
        Sierra::SwapchainPresentationMode preferredPresentationMode = Sierra::SwapchainPresentationMode::VSync;

        const Sierra::WindowManager &windowManager;
        const Sierra::RenderingContext &renderingContext;
    };

    class SIERRA_ENGINE_API Surface
    {
    public:
        /* --- POLLING METHODS --- */
        virtual void Update() const = 0;
        virtual void Present(Sierra::CommandBuffer &commandBuffer) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const Sierra::Image& GetCurrentSwapchainImage() const = 0;

        [[nodiscard]] virtual Sierra::Window& GetWindow() const = 0;
        [[nodiscard]] virtual Sierra::Swapchain& GetSwapchain() const = 0;

        /* --- OPERATORS --- */
        Surface(const Surface&) = delete;
        Surface& operator=(const Surface&) = delete;
        
        /* --- DESTRUCTOR --- */
        virtual ~Surface() = default;
    
    protected:
        explicit Surface(const SurfaceCreateInfo &createInfo);

    };

}
