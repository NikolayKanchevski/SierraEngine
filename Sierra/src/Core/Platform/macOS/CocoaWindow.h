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
#include "../../Window.h"

#include "CocoaContext.h"
#include "CocoaInputManager.h"
#include "CocoaCursorManager.h"

namespace Sierra
{

    class SIERRA_API CocoaWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaWindow(CocoaContext &cocoaContext, const WindowCreateInfo &createInfo);

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

        [[nodiscard]] Screen& GetScreen() const override;
        [[nodiscard]] InputManager& GetInputManager() override;
        [[nodiscard]] CursorManager& GetCursorManager() override;
        [[nodiscard]] PlatformAPI GetAPI() const override;

        [[nodiscard]] const NSView* GetNSView() const { return view; }
        [[nodiscard]] const NSWindow* GetNSWindow() const { return window; }

        /* --- DESTRUCTOR --- */
        ~CocoaWindow() override;

    private:
        CocoaContext &cocoaContext;

        NSView* view = nil;
        void* /* CocoaWindowDelegate* */ delegate = nil;
        NSWindow* window = nil;

        CocoaInputManager inputManager;
        CocoaCursorManager cursorManager;

        std::string title;
        bool maximized = false;
        bool closed = false;

        [[nodiscard]] uint32 GetTitleBarHeight() const;

    #if defined(__OBJC__) && defined(COCOA_WINDOW_IMPLEMENTATION)
        public:
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