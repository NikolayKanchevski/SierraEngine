//
// Created by Nikolay Kanchevski on 11.12.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the WaylandCursorManager.h file is only allowed in Linux builds!"
#endif

#include "../../CursorManager.h"

#include "WaylandContext.h"

namespace Sierra
{

    class WaylandWindow;

    class SIERRA_API WaylandCursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit WaylandCursorManager(const WaylandContext& waylandContext, const WaylandWindow& window);

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
        WaylandCursorManager(const WaylandCursorManager&) = delete;
        WaylandCursorManager& operator=(const WaylandCursorManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        WaylandCursorManager(WaylandCursorManager&&) = delete;
        WaylandCursorManager& operator=(WaylandCursorManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WaylandCursorManager() noexcept override;

    private:
        struct Callbacks;

        const WaylandContext& waylandContext;
        const WaylandWindow& window;

        Vector2Int cursorPosition = { 0, 0 };
        Vector2Int lastCursorPosition = { 0, 0 };

        bool cursorShown = true;
        bool cursorInside = false;

        // A hidden cursor is locked in place, and has its motion reported relatively instead
        zwp_locked_pointer_v1* lockedPointer = nullptr;
        zwp_relative_pointer_v1* relativePointer = nullptr;
        Vector2 relativeMotion = { 0, 0 };

        friend class WaylandWindow;
        void Update();

        void RegisterCursorEnter(Vector2Int position);
        void RegisterCursorLeave();

        void ApplyCursor() const noexcept;
        void LockPointer();
        void UnlockPointer() noexcept;

    };

}
