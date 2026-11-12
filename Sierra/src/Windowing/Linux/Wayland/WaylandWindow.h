//
// Created by Nikolay Kanchevski on 10.8.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the WaylandWindow.h file is only allowed in Linux builds!"
#endif

#include "../../Window.h"

#include "WaylandContext.h"
#include "WaylandInputManager.h"
#include "WaylandCursorManager.h"

namespace Sierra
{

    class SIERRA_API WaylandWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WaylandWindow(WaylandContext& waylandContext, const WindowCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Minimize() override;
        void Maximize() override;
        void Restore();
        void Show() override;
        void Hide() override;
        void RequestAttention();
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(std::string_view title) override;
        void SetPosition(Vector2Int position) override;
        void SetSize(Vector2UInt size) override;
        void SetSizeLimits(Vector2UInt minimumSize, Vector2UInt maximumSize);
        void SetAspectRatio(uint32 numerator, uint32 denominator);
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetTitle() const noexcept override;
        [[nodiscard]] float32 GetOpacity() const noexcept override;
        [[nodiscard]] Vector2Int GetPosition() const noexcept override;

        [[nodiscard]] uint32 GetWidth() const noexcept override;
        [[nodiscard]] uint32 GetHeight() const noexcept override;
        [[nodiscard]] uint32 GetFramebufferWidth() const noexcept override;
        [[nodiscard]] uint32 GetFramebufferHeight() const noexcept override;
        [[nodiscard]] float32 GetScaling() const noexcept;

        [[nodiscard]] bool IsVisible() const noexcept override;
        [[nodiscard]] bool IsTransparent() const noexcept override;
        [[nodiscard]] bool IsMinimized() const noexcept override;
        [[nodiscard]] bool IsMaximized() const noexcept override;
        [[nodiscard]] bool IsFocused() const noexcept override;
        [[nodiscard]] bool IsClosed() const noexcept override;

        [[nodiscard]] InputManager* GetInputManager() noexcept override { return &inputManager; }
        [[nodiscard]] const InputManager* GetInputManager() const noexcept override { return &inputManager; }

        [[nodiscard]] CursorManager* GetCursorManager() noexcept override { return &cursorManager; }
        [[nodiscard]] const CursorManager* GetCursorManager() const noexcept override { return &cursorManager; }

        [[nodiscard]] TouchManager* GetTouchManager() noexcept override { return nullptr;  }
        [[nodiscard]] const TouchManager* GetTouchManager() const noexcept override { return nullptr;  }

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Wayland; }

        [[nodiscard]] wl_surface* GetSurface() const noexcept { return surface; }
        [[nodiscard]] wl_display* GetDisplay() const noexcept { return waylandContext.GetDisplay(); }
        [[nodiscard]] const WaylandScreen* GetScreen() const noexcept;

        /* --- COPY SEMANTICS --- */
        WaylandWindow(const WaylandWindow&) = delete;
        WaylandWindow& operator=(const WaylandWindow&) = delete;

        /* --- MOVE SEMANTICS --- */
        WaylandWindow(WaylandWindow&&) = delete;
        WaylandWindow& operator=(WaylandWindow&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WaylandWindow() noexcept override;

    private:
        struct Callbacks;
        WaylandContext& waylandContext;

        WaylandInputManager inputManager;
        WaylandCursorManager cursorManager;

        std::string title = { };
        bool resizable = false;
        bool transparent = false;

        wl_surface* surface = nullptr;
        std::vector<wl_output*> outputs = { };

        // Decorated windows are managed by libdecor, or otherwise by the shell directly, when it is unavailable (never both)
        libdecor_frame* libdecorFrame = nullptr;
        xdg_surface* xdgSurface = nullptr;
        xdg_toplevel* xdgToplevel = nullptr;
        zxdg_toplevel_decoration_v1* xdgDecoration = nullptr;
        xdg_activation_token_v1* activationToken = nullptr;

        wp_viewport* viewport = nullptr;
        wp_fractional_scale_v1* fractionalScale = nullptr;
        uint32 fractionalScaleNumerator = 120;
        int32 bufferScale = 1;

        Vector2UInt size = { 0, 0 };
        Vector2UInt framebufferSize = { 0, 0 };
        Vector2UInt floatingSize = { 0, 0 };

        Vector2UInt minimumSize = { 0, 0 };
        Vector2UInt maximumSize = { 0, 0 };
        uint32 aspectRatioNumerator = 0;
        uint32 aspectRatioDenominator = 0;

        struct ToplevelState
        {
            Vector2UInt size = { 0, 0 };
            bool maximized = false;
            bool fullscreen = false;
        };
        ToplevelState pendingToplevelState = { };

        bool configured = false;
        bool maximized = false;
        bool fullscreen = false;
        bool focused = false;

        bool closeRequested = false;
        bool closed = false;

        void CreateShellObjects();
        void DestroyShellObjects() noexcept;

        void Configure(Vector2UInt newSize, bool newMaximized, bool newFullscreen);
        bool Resize(Vector2UInt newSize);

        void ApplySizeLimits() const noexcept;
        void RequestActivation(bool takeFocus);

        void UpdateBufferScale();
        void UpdateFramebufferSize();
        void UpdateOpaqueRegion() const noexcept;

        [[nodiscard]] bool IsFloating() const noexcept { return !maximized && !fullscreen; }
        [[nodiscard]] Vector2UInt GetAspectRatioFittedSize(Vector2UInt targetSize) const noexcept;

        // Input and screens are tracked by the context, which notifies the windows they concern
        friend class WaylandContext;

        void HandlePointerEnter(Vector2 position);
        void HandlePointerLeave();
        void HandlePointerMotion(Vector2 position);
        void HandlePointerButton(uint32 buttonCode, bool pressed);
        void HandlePointerScroll(Vector2 scroll);

        void HandleKeyboardEnter();
        void HandleKeyboardLeave();
        void HandleKey(uint32 scancode, bool pressed);

        void HandleScreenRemoval(const WaylandScreen& screen);

    };

}
