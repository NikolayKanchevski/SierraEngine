//
// Created by Nikolay Kanchevski on 17.08.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaWindow.h file is only allowed in macOS builds!"
#endif

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using NSView = void;
        using NSWindow = void;
    }
#endif

#include "../Window.h"

#include "CocoaInputManager.h"
#include "CocoaCursorManager.h"
#include "../../Platform/macOS/CocoaContext.h"

namespace Sierra
{

    class SIERRA_API CocoaWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        CocoaWindow(const CocoaContext& cocoaContext, const WindowCreateInfo& createInfo);

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

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override { return WindowingBackendType::Cocoa; }

        [[nodiscard]] const NSWindow* GetNSWindow() const noexcept { return window; }
        [[nodiscard]] const NSView* GetNSView() const noexcept { return view; }

        /* --- COPY SEMANTICS --- */
        CocoaWindow(const CocoaWindow&) = delete;
        CocoaWindow& operator=(const CocoaWindow&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaWindow(CocoaWindow&&) noexcept = default;
        CocoaWindow& operator=(CocoaWindow&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~CocoaWindow() noexcept override;

    private:
        const CocoaContext* cocoaContext;

        void* /* CocoaWindowDelegate* */ delegate = nil;
        NSWindow* window = nil;
        NSView* view = nil;

        CocoaInputManager inputManager;
        CocoaCursorManager cursorManager;

        bool maximized = false;
        bool closed = false;

        [[nodiscard]] uint32 GetTitleBarHeight() const;

    public:
    #if defined(__OBJC__) && defined(COCOA_WINDOW_IMPLEMENTATION)
        /* --- EVENTS --- */
        void WindowShouldClose();
        void WindowDidResize(const NSNotification* notification);
        void WindowDidMove(const NSNotification* notification);
        void WindowDidMiniaturize(const NSNotification* notification);
        void WindowDidBecomeKey(const NSNotification* notification);
        void WindowDidResignKey(const NSNotification* notification);
        void WindowDidChangeBackingProperties();
    #endif

    };

}