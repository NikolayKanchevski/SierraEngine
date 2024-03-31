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
        explicit X11Window(const X11Context &x11Context, const WindowCreateInfo &createInfo);

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
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetTitle() const override;
        [[nodiscard]] Vector2Int GetPosition() const override;
        [[nodiscard]] uint32 GetWidth() const override;
        [[nodiscard]] uint32 GetHeight() const override;
        [[nodiscard]] uint32 GetFramebufferWidth() const override;
        [[nodiscard]] uint32 GetFramebufferHeight() const override;
        [[nodiscard]] float32 GetOpacity() const override;
        [[nodiscard]] bool IsClosed() const override;
        [[nodiscard]] bool IsMinimized() const override;
        [[nodiscard]] bool IsMaximized() const override;
        [[nodiscard]] bool IsFocused() const override;
        [[nodiscard]] bool IsHidden() const override;

        [[nodiscard]] const Screen& GetScreen() const override;
        [[nodiscard]] InputManager& GetInputManager() override;
        [[nodiscard]] CursorManager& GetCursorManager() override;
        [[nodiscard]] PlatformAPI GetAPI() const override;

        [[nodiscard]] inline XID GetX11Window() const { return window; }
        [[nodiscard]] inline Display* GetDisplay() const { return x11Context.GetDisplay(); }

        /* --- DESTRUCTOR --- */
        ~X11Window() override;

    private:
        const X11Context &x11Context;
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

        struct WindowEventQueue
        {
            XID window = 0;
            std::queue<XEvent> queue = { };
        };
        static inline std::vector<WindowEventQueue> unhandledEventQueues;
        void HandleX11Event(XEvent &event);

    };

}
