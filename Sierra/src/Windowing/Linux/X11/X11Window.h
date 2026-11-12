//
// Created by Nikolay Kanchevski on 8.26.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Window.h file is only allowed in Linux builds!"
#endif

#include "../../Window.h"

#include "X11Context.h"
#include "X11InputManager.h"
#include "X11CursorManager.h"

namespace Sierra
{

    class SIERRA_API X11Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Window(X11Context& x11Context, XID window, const WindowCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        void HandleX11Event(XEvent& event);

        /* --- SETTER METHODS --- */
        void SetTitle(std::string_view title) override;
        void SetPosition(Vector2Int position) override;
        void SetSize(Vector2UInt size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetTitle() const noexcept override;
        [[nodiscard]] Vector2Int GetPosition() const noexcept override;
        [[nodiscard]] uint32 GetWidth() const noexcept override;
        [[nodiscard]] uint32 GetHeight() const noexcept override;
        [[nodiscard]] uint32 GetFramebufferWidth() const noexcept override;
        [[nodiscard]] uint32 GetFramebufferHeight() const noexcept override;
        [[nodiscard]] float32 GetOpacity() const noexcept override;

        [[nodiscard]] bool IsClosed() const noexcept override;
        [[nodiscard]] bool IsMinimized() const noexcept override;
        [[nodiscard]] bool IsMaximized() const noexcept override;
        [[nodiscard]] bool IsFocused() const noexcept override;
        [[nodiscard]] bool IsVisible() const noexcept override;
        [[nodiscard]] bool IsTransparent() const noexcept override;

        [[nodiscard]] InputManager* GetInputManager() noexcept override { return &inputManager; }
        [[nodiscard]] const InputManager* GetInputManager() const noexcept override { return &inputManager; }

        [[nodiscard]] CursorManager* GetCursorManager() noexcept override { return &cursorManager; }
        [[nodiscard]] const CursorManager* GetCursorManager() const noexcept override { return &cursorManager; }

        [[nodiscard]] TouchManager* GetTouchManager() noexcept override { return nullptr;  }
        [[nodiscard]] const TouchManager* GetTouchManager() const noexcept override { return nullptr;  }

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::X11; }

        [[nodiscard]] XID GetX11Window() const noexcept { return window; }
        [[nodiscard]] Display* GetDisplay() const noexcept { return x11Context.GetDisplay(); }

        /* --- MOVE SEMANTICS --- */
        X11Window(X11Window&&) = delete;
        X11Window& operator=(X11Window&&) = delete;

        /* --- COPY SEMANTICS --- */
        X11Window(const X11Window&) = delete;
        X11Window& operator=(const X11Window&) = delete;

        /* --- DESTRUCTOR --- */
        ~X11Window() noexcept override;

    private:
        X11Context& x11Context;
        XID window;

        X11InputManager inputManager;
        X11CursorManager cursorManager;

        std::string title = { };
        bool resizable = false;

        // Decorations, which the window manager draws around the window: [ Left | Right | Top | Bottom ]
        Vector4Int extents = { 0, 0, 0, 0 };

        // Size of the window's content in physical pixels, which is kept up to date with the server, as it is needed often
        Vector2UInt size = { 0, 0 };
        Vector2Int lastPosition = { 0, 0 };

        bool closed = false;
        bool mapped = false;
        bool focused = false;
        bool minimized = false;
        bool maximized = false;

        void SetSizeHints(Vector2UInt fixedSize) const noexcept;
        void ApplyTitle() const noexcept;
        void ReadFrameExtents() noexcept;
        [[nodiscard]] bool ReadMinimized() const noexcept;
        [[nodiscard]] bool ReadMaximized() const noexcept;
        [[nodiscard]] bool IsWindowStateSupported() const noexcept;
        [[nodiscard]] Vector2Int QueryPosition() const noexcept;
        void Destroy() noexcept;

    };

}
