//
// Created by Nikolay Kanchevski on 3.10.23.
//

#pragma once

#if !SR_PLATFORM_MACOS
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

    struct CocoaCursorManagerCreateInfo final : public CursorManagerCreateInfo
    {
        const NSWindow* window;
    };

    class SIERRA_API CocoaCursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaCursorManager(const CocoaCursorManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate();
        void OnUpdateEnd();

        /* --- SETTER METHODS --- */
        void SetCursorPosition(const Vector2 &position) override;
        void ShowCursor() override;
        void HideCursor() override;

        /* --- GETTER METHODS --- */
        Vector2 GetCursorPosition() override;
        bool IsCursorHidden() override;

        float32 GetHorizontalDelta() override;
        float32 GetVerticalDelta() override;

        /* --- EVENTS --- */
        #if defined(__OBJC__)
            void MouseMoved(const NSEvent* event);
        #endif

    private:
        const NSWindow* window;

        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };

        bool cursorHidden = false;
        bool justHidCursor = false;

    };

}
