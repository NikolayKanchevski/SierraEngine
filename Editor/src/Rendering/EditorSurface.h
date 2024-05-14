//
// Created by Nikolay Kanchevski on 14.05.24.
//

#pragma once

namespace SierraEngine
{

    struct EditorSurfaceCreateInfo
    {
        const Sierra::WindowManager &windowManager;
        const Sierra::RenderingContext &renderingContext;
    };

    class EditorSurface
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorSurface(const EditorSurfaceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update() const;
        void Present(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer) const;

        /* --- SETTER METHODS --- */

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsActive() const { return !window->IsClosed();  }

        [[nodiscard]] inline uint32 GetCurrentFrameIndex() const { return swapchain->GetCurrentFrameIndex(); }
        [[nodiscard]] inline uint32 GetCurrentImageIndex() const { return swapchain->GetCurrentImageIndex(); }
        [[nodiscard]] inline uint32 GetConcurrentFrameCount() const { return swapchain->GetConcurrentFrameCount(); }

        [[nodiscard]] inline uint32 GetWidth() const { return swapchain->GetWidth(); };
        [[nodiscard]] inline uint32 GetHeight() const { return swapchain->GetHeight(); };
        [[nodiscard]] inline uint32 GetScaling() const { return swapchain->GetScaling(); };

        [[nodiscard]] inline const std::unique_ptr<Sierra::Image>& GetCurrentOutputImage() const { return stagingImages[swapchain->GetCurrentImageIndex()]; }
        [[nodiscard]] inline const std::unique_ptr<Sierra::Image>& GetCurrentSwapchainImage() const { return swapchain->GetCurrentImage(); }

        [[nodiscard]] inline const Sierra::Screen& GetScreen() const { return window->GetScreen(); }
        [[nodiscard]] inline Sierra::InputManager& GetInputManager() const { return window->GetInputManager(); }
        [[nodiscard]] inline Sierra::CursorManager& GetCursorManager() const { return window->GetCursorManager(); }
        [[nodiscard]] inline Sierra::TouchManager& GetTouchManager() const { return window->GetTouchManager(); }

        /* --- OPERATORS --- */
        EditorSurface(const EditorSurface&) = delete;
        EditorSurface& operator=(const EditorSurface&) = delete;

        /* --- DESTRUCTOR --- */
        ~EditorSurface() = default;

    private:
        const Sierra::RenderingContext &renderingContext;

        std::unique_ptr<Sierra::Window> window = nullptr;
        std::unique_ptr<Sierra::Swapchain> swapchain = nullptr;

        std::vector<std::unique_ptr<Sierra::Image>> stagingImages = { };
        void CreateStagingImages();

    };

}
