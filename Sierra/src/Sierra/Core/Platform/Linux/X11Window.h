//
// Created by Nikolay Kanchevski on 8.26.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11Window.h file is only allowed in Linux builds!"
#endif

#include "../../Window.h"
#include "X11Context.h"
#include "X11Screen.h"
#include "X11InputManager.h"
#include "X11CursorManager.h"

namespace Sierra
{

    class SIERRA_API X11Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11Window(const WindowCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;
        void Minimize() override;
        void Maximize() override;
        void Show() override;
        void Hide() override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(const String &title) override;
        void SetPosition(const Vector2Int &position) override;
        void SetSize(const Vector2UInt &size) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        String GetTitle() const override;
        Vector2Int GetPosition() const override;
        Vector2UInt GetSize() const override;
        Vector2UInt GetFramebufferSize() const override;
        float32 GetOpacity() const override;
        bool IsClosed() const override;
        bool IsMinimized() const override;
        bool IsMaximized() const override;
        bool IsFocused() const override;
        bool IsHidden() const override;

        Screen& GetScreen() override;
        InputManager& GetInputManager() override;
        CursorManager& GetCursorManager() override;
        WindowAPI GetAPI() const override;

        /* --- DESTRUCTOR --- */
        ~X11Window() override;

    private:
        const X11Context &x11Context;
        XID window;

        X11Screen* screen = nullptr;
        X11InputManager inputManager;
        X11CursorManager cursorManager;

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
        static inline std::unordered_map<XID, std::vector<XEvent>> unhandledEvents;
        void HandleX11Event(XEvent &event);

    };

}
