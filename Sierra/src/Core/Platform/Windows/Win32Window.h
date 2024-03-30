//
// Created by Nikolay Kanchevski on 8.21.2023.
//
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Window.h file is only allowed in Windows builds!"
#endif

#include "../../Window.h"
#include "Win32Context.h"
#include "Win32InputManager.h"
#include "Win32CursorManager.h"

namespace Sierra
{

    class SIERRA_API Win32Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32Window(const Win32Context &win32Context, const WindowCreateInfo &createInfo);

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

        [[nodiscard]] inline HWND GetHwnd() const { return window; }
        [[nodiscard]] inline HINSTANCE GetHInstance() const { return win32Context.GetHInstance(); }

        /* --- DESTRUCTOR --- */
        ~Win32Window() override;

    private:
        const Win32Context &win32Context;

        HWND window;

        Win32InputManager inputManager;
        Win32CursorManager cursorManager;

        std::string title;
        bool closed = false;

        bool justBecameShown = false;      // This is to prevent all the made up events the window manager sends when showing a hidden window
        bool nextMoveEventBlocked = false; // This is to prevent detecting when window manager moves window to [x: ~-32000, y: ~-32000] when hiding/minimizing it
        static LRESULT CALLBACK WindowProc(HWND callingWindow, UINT message, WPARAM wParam, LPARAM lParam);

    };

}
