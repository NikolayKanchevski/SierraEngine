//
// Created by Nikolay Kanchevski on 8.26.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Window.h file is only allowed in Linux builds!"
#endif

#include "../Window.h"

#include "X11InputManager.h"
#include "X11CursorManager.h"
#include "../../Platform/Linux/X11Context.h"

namespace Sierra
{

    class SIERRA_API X11Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Window(const X11Context& x11Context, const WindowCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(std::string_view title) override;
        void SetPosition(Vector2Int position) override;
        void SetSize(Vector2UInt size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTitle() const noexcept override;
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
        [[nodiscard]] bool IsHidden() const noexcept override;

        [[nodiscard]] InputManager* GetInputManager() noexcept override;
        [[nodiscard]] CursorManager* GetCursorManager() noexcept override;
        [[nodiscard]] TouchManager* GetTouchManager() noexcept override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override;

        [[nodiscard]] XID GetX11Window() const { return window; }
        [[nodiscard]] Display* GetDisplay() const { return x11Context.GetDisplay(); }

        /* --- MOVE SEMANTICS --- */
        X11Window(X11Window&&) noexcept = delete;
        X11Window& operator=(X11Window&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        X11Window(const X11Window&) = delete;
        X11Window& operator=(const X11Window&) = delete;

        /* --- DESTRUCTOR --- */
        ~X11Window() noexcept override;

    private:
        const X11Context& x11Context;
        XID window;

        X11InputManager inputManager;
        X11CursorManager cursorManager;

        std::string title;
        Vector4Int extents = { 0, 0, 0, 0 }; // [ Left | Right | Top | Bottom ]
        bool closed = false;

        Vector2Int lastPosition = { std::numeric_limits<int32>::max(), std::numeric_limits<int32>::max() };
        Vector2UInt lastSize = { std::numeric_limits<uint32>::max(), std::numeric_limits<uint32>::max() };

        bool lastMinimizedState = false;
        bool lastMaximizedState = false;

        bool resizable = false;
        bool shouldMaximizeOnShow = false;

        static void WindowEventCallback(const XEvent& event, XID windowID, void* userData);
        void HandleX11Event(XEvent& event);
        [[nodiscard]] Vector4UInt GetExtents();

    };

}
