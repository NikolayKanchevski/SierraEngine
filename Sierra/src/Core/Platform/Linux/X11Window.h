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
        explicit X11Window(const X11Context &context, const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(const std::string &title) override;
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const std::string& GetTitle() const override;
        [[nodiscard]] Vector2Int GetPosition() const override;
        [[nodiscard]] Vector2UInt GetSize() const override;
        [[nodiscard]] Vector2UInt GetFramebufferSize() const override;
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

        /*
         * Since you cannot get events for a specific window with X11 (you instead get all pending events for all windows),
         * we store the events, which are not for the current window, so they can be handled later - when the corresponding window is updated
        */
        // TODO: Make use of queues
        static inline std::unordered_map<XID, std::vector<XEvent>> unhandledEvents;
        void HandleX11Event(XEvent &event);

    };

}
