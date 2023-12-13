//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaCursorManager.h file is only allowed in macOS builds!"
#endif

#include "../../CursorManager.h"

#if !defined(__OBJC__)
    namespace Sierra
    {
        typedef void NSWindow;
    }
#else
    #include <Cocoa/Cocoa.h>
#endif

namespace Sierra
{

    class SIERRA_API CocoaCursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaCursorManager(const NSWindow* window, const CursorManagerCreateInfo &createInfo);

        /* --- SETTER METHODS --- */
        void ShowCursor() override;
        void HideCursor() override;
        void SetCursorPosition(const Vector2 &position) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsCursorHidden() const override;
        [[nodiscard]] Vector2 GetCursorPosition() const override;
        [[nodiscard]] float32 GetHorizontalDelta() const override;
        [[nodiscard]] float32 GetVerticalDelta() const override;

    private:
        const NSWindow* window;

        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };

        bool cursorHidden = false;
        bool justHidCursor = false;

        friend class CocoaWindow;
        void OnUpdate();
        void OnUpdateEnd();

        #if defined(__OBJC__) && (defined(COCOA_CURSOR_MANAGER_IMPLEMENTATION) || defined(COCOA_WINDOW_IMPLEMENTATION))
            /* --- EVENTS --- */
            public:
                void MouseMoved(const NSEvent* event);
        #endif
    };

}
