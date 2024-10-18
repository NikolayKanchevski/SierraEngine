//
// Created by Nikolay Kanchevski on 10.28.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11CursorManager.h file is only allowed in Linux builds!"
#endif

#include "../CursorManager.h"

#include "../../Platform/Linux/X11Context.h"

namespace Sierra
{

    class SIERRA_API X11CursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11CursorManager(const X11Context& x11Context, XID window);

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
        X11CursorManager(const X11CursorManager&) = delete;
        X11CursorManager& operator=(const X11CursorManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        X11CursorManager(X11CursorManager&&) = delete;
        X11CursorManager& operator=(X11CursorManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~X11CursorManager() noexcept override = default;

    private:
        const X11Context& x11Context;
        const XID window;

        Vector2Int cursorPosition = { 0, 0 };
        Vector2Int lastCursorPosition = { 0, 0 };

        bool cursorShown = true;
        bool justHidCursor = false;

        friend class X11Window;
        void Update();

    };

}
