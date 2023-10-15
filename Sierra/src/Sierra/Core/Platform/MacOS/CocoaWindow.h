//
// Created by Nikolay Kanchevski on 17.08.23.
//

#pragma once

#if !SR_PLATFORM_MACOS
    #error "Including the CocoaWindow.h file is only allowed in macOS builds!"
#endif

#include "../../Window.h"
#include "MacOSInstance.h"
#include "CocoaInputManager.h"
#import "CocoaCursorManager.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void CocoaWindowDelegate;
        typedef void CocoaWindowContentView;
        typedef void CocoaWindowImplementation;
    }
#else
    #include <Cocoa/Cocoa.h>
    @class CocoaWindowDelegate;
    @class CocoaWindowContentView;
    @class CocoaWindowImplementation;
#endif

namespace Sierra
{

    class SIERRA_API CocoaWindow final : public Window
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaWindow(const WindowCreateInfo &createInfo);

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

        InputManager& GetInputManager() override;
        CursorManager& GetCursorManager() override;
        WindowAPI GetAPI() const override;

        /* --- EVENTS --- */
        #if defined(__OBJC__)
            void WindowShouldClose();
            void WindowDidResize(const NSNotification* notification);
            void WindowDidMove(const NSNotification* notification);
            void WindowDidMiniaturize(const NSNotification* notification);
            void WindowDidBecomeKey(const NSNotification* notification);
            void WindowDidResignKey(const NSNotification* notification);
        #endif

        /* --- DESTRUCTOR --- */
        ~CocoaWindow();

    private:
        MacOSInstance &macOSInstance;

        CocoaInputManager* inputManager; // A raw pointer, as ObjectiveC++ is doing some questionable automatic frees, causing segfaults when not managed manually
        CocoaCursorManager cursorManager;

        bool maximized = false;
        bool closed = false;

        CocoaWindowDelegate* delegate = nullptr;
        CocoaWindowContentView* view = nullptr;
        CocoaWindowImplementation* window = nullptr;

        float32 GetTitleBarHeight() const;

    };

}