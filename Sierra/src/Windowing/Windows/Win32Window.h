//
// Created by Nikolay Kanchevski on 8.21.2023.
//
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Window.h file is only allowed in Windows builds!"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../Window.h"
#include "Win32InputManager.h"
#include "Win32CursorManager.h"
#include "../../Platform/Windows/Win32Context.h"

namespace Sierra
{

    class SIERRA_API Win32Window final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32Window(const Win32Context& win32Context, const WindowCreateInfo& createInfo);

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

        [[nodiscard]] HWND GetHwnd() const { return window; }
        [[nodiscard]] HINSTANCE GetHInstance() const { return win32Context.GetHInstance(); }

        /* --- MOVE SEMANTICS --- */
        Win32Window(Win32Window&&) noexcept = delete;
        Win32Window& operator=(Win32Window&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        Win32Window(const Win32Window&) = delete;
        Win32Window& operator=(const Win32Window&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32Window() noexcept override;

    private:
        const Win32Context& win32Context;
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
