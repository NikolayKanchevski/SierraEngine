//
// Created by Nikolay Kanchevski on 3.10.23.
//

#if !SR_PLATFORM_MACOS
    #error "Including the CocoaCursorManager.h file is only allowed in macOS builds!"
#endif

#pragma once

#include "../../CursorManager.h"

#if defined(__OBJC__)
    #include <Cocoa/Cocoa.h>
#endif

namespace Sierra
{

    class SIERRA_API CocoaCursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit CocoaCursorManager(const CursorManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate() override;

        /* --- SETTER METHODS --- */
        void SetCursorPosition(const Vector2 &position) override;
        void ShowCursor() override;
        void HideCursor() override;

        /* --- GETTER METHODS --- */
        Vector2 GetCursorPosition() override;
        bool IsCursorShown() override;
        bool IsCursorHidden() override;

        float GetHorizontalDelta() override;
        float GetVerticalDelta() override;

        /* --- EVENTS --- */
        #if defined(__OBJC__)
            void MouseMoved(const NSEvent* event);
        #endif

    private:
        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };
        bool cursorHidden = false;

        double InvertWindowPositionY(double yPosition);

    };

}
