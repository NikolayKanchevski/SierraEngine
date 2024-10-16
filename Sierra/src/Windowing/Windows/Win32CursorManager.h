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
        explicit Win32CursorManager(HWND window);

        /* --- POLLING METHODS --- */
        void RegisterCursorMove(Vector2 position) override;

        /* --- SETTER METHODS --- */
        void SetCursorVisibility(bool visible) override;
        void SetCursorPosition(Vector2 position) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsCursorVisible() const override;
        [[nodiscard]] Vector2 GetCursorPosition() const override;
        [[nodiscard]] Vector2 GetCursorDelta() const override;

    private:
        HWND window;
        Vector2 cursorPosition = { 0, 0 };
        Vector2 lastCursorPosition = { 0, 0 };

        bool cursorShown = true;
        bool justHidCursor = false;

        friend class Win32Window;
        void Update();
        void PostUpdate();

    };

}
