//
// Created by Nikolay Kanchevski on 10.20.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32CursorManager.h file is only allowed in Windows builds!"
#endif

#include "../../CursorManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Sierra
{

    class SIERRA_API Win32CursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32CursorManager(const CursorManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void OnUpdate();

        /* --- SETTER METHODS --- */
        void SetCursorPosition(const Vector2 &position) override;
        void ShowCursor() override;
        void HideCursor() override;

        /* --- GETTER METHODS --- */
        Vector2 GetCursorPosition() override;
        bool IsCursorShown() override;
        bool IsCursorHidden() override;

        float32 GetHorizontalDelta() override;
        float32 GetVerticalDelta() override;

        /* --- EVENTS --- */
        void MouseMoveMessage(UINT message, WPARAM wParam, LPARAM lParam);

    private:
        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };
        bool cursorShown = true;

    };

}
