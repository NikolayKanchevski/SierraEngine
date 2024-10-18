//
// Created by Nikolay Kanchevski on 10.20.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32CursorManager.h file is only allowed in Windows builds!"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../CursorManager.h"

namespace Sierra
{

    class SIERRA_API Win32CursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32CursorManager(HWND window);

        /* --- POLLING METHODS --- */
        void RegisterCursorMove(Vector2Int position) override;

        /* --- SETTER METHODS --- */
        void SetCursorVisibility(bool visible) override;
        void SetCursorPosition(Vector2Int position) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsCursorVisible() const noexcept override;
        [[nodiscard]] Vector2Int GetCursorPosition() const noexcept override;
        [[nodiscard]] Vector2 GetCursorDelta() const noexcept override;
        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override;

        /* --- COPY SEMANTICS --- */
        Win32CursorManager(const Win32CursorManager&) = delete;
        Win32CursorManager& operator=(const Win32CursorManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32CursorManager(Win32CursorManager&&) = delete;
        Win32CursorManager& operator=(Win32CursorManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32CursorManager() noexcept override = default;

    private:
        HWND window;
        Vector2Int cursorPosition = { 0, 0 };
        Vector2Int lastCursorPosition = { 0, 0 };

        bool cursorShown = true;
        bool justHidCursor = false;

        friend class Win32Window;
        void Update();

    };

}
