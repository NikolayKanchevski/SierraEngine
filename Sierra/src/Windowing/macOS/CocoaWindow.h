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

        [[nodiscard]] const NSView* GetNSView() const noexcept { return view; }
        [[nodiscard]] const NSWindow* GetNSWindow() const noexcept { return window; }

        /* --- MOVE SEMANTICS --- */
        CocoaWindow(CocoaWindow&&) noexcept = delete;
        CocoaWindow& operator=(CocoaWindow&&) noexcept = delete;

        /* --- COPY SEMANTICS --- */
        CocoaWindow(const CocoaWindow&) = delete;
        CocoaWindow& operator=(const CocoaWindow&) = delete;

        /* --- DESTRUCTOR --- */
        ~CocoaWindow() noexcept override;

    private:
        const CocoaContext& cocoaContext;

        NSView* view = nil;
        void* /* CocoaWindowDelegate* */ delegate = nil;
        NSWindow* window = nil;

        CocoaInputManager inputManager;
        CocoaCursorManager cursorManager;

        std::string title;
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
    #endif

    };

}