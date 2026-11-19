//
// Created by Nikolay Kanchevski on 8.21.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32Window.h file is only allowed in Windows builds!"
#endif

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
        explicit Win32Window(Win32Context& win32Context, const WindowCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update() override;
        void Minimize() override;
        void Maximize() override;
        void Restore() override;
        void Show() override;
        void Hide() override;
        void RequestFocus(WindowFocusRequestType requestType) override;
        void Focus() override;
        void Close() override;

        /* --- SETTER METHODS --- */
        void SetTitle(std::string_view title) override;
        void SetPosition(Vector2Int position) override;
        void SetSize(Vector2UInt size) override;
        void SetSizeLimits(Vector2UInt minimumSize, Vector2UInt maximumSize) override;
        void SetAspectRatio(uint32 numerator, uint32 denominator) override;
        void SetOpacity(float32 opacity) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetTitle() const noexcept override;
        [[nodiscard]] Vector2Int GetPosition() const noexcept override;
        [[nodiscard]] float32 GetOpacity() const noexcept override;

        [[nodiscard]] Vector2UInt GetSize() const noexcept override;
        [[nodiscard]] Vector2UInt GetFramebufferSize() const noexcept override;
        [[nodiscard]] float32 GetScaling() const noexcept override;

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

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Win32; }

        [[nodiscard]] HWND GetHwnd() const noexcept { return window; }
        [[nodiscard]] HINSTANCE GetHInstance() const noexcept { return win32Context.GetHInstance(); }

        /* --- MOVE SEMANTICS --- */
        Win32Window(Win32Window&&) = delete;
        Win32Window& operator=(Win32Window&&) = delete;

        /* --- COPY SEMANTICS --- */
        Win32Window(const Win32Window&) = delete;
        Win32Window& operator=(const Win32Window&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32Window() noexcept override;

    private:
        Win32Context& win32Context;
        HWND window = nullptr;

        Win32InputManager inputManager;
        Win32CursorManager cursorManager;

        std::string title = { };
        bool resizable = false;

        // DPI of the monitor the window is on, which its logical units get scaled by (96 being an unscaled one)
        UINT dpi = USER_DEFAULT_SCREEN_DPI;

        Vector2UInt size = { 0, 0 };
        Vector2Int lastPosition = { 0, 0 };

        Vector2UInt minimumSize = { 0, 0 };
        Vector2UInt maximumSize = { 0, 0 };
        Vector2UInt aspectRatio = { 0, 0 };

        bool closed = false;
        bool focused = false;
        bool minimized = false;
        bool maximized = false;

        [[nodiscard]] DWORD GetStyle() const noexcept;
        [[nodiscard]] DWORD GetExStyle() const noexcept;
        [[nodiscard]] float32 GetDPIScaling() const noexcept;

        [[nodiscard]] Vector2UInt ToPhysicalSize(Vector2UInt logicalSize) const noexcept;
        [[nodiscard]] Vector2UInt ToLogicalSize(Vector2UInt physicalSize) const noexcept;
        [[nodiscard]] Vector2Int ToPhysicalPosition(Vector2Int logicalPosition) const noexcept;
        [[nodiscard]] Vector2Int ToLogicalPosition(Vector2Int physicalPosition) const noexcept;

        [[nodiscard]] Vector2UInt GetFrameSize(Vector2UInt physicalSize) const noexcept;
        [[nodiscard]] Vector2UInt GetConstrainedSize(Vector2UInt targetSize) const noexcept;
        [[nodiscard]] Vector2Int QueryPosition() const noexcept;
        [[nodiscard]] Vector2UInt QuerySize() const noexcept;

        void ApplyAspectRatio(WPARAM edge, RECT& frameRect) const noexcept;
        void ReleaseStuckKeys();
        void Destroy() noexcept;

        friend class Win32Context;
        [[nodiscard]] LRESULT HandleWin32Message(UINT message, WPARAM wParam, LPARAM lParam);
        void HandleKeyMessage(UINT message, WPARAM wParam, LPARAM lParam);

    };

}
