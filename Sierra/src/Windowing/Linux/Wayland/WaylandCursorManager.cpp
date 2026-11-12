//
// Created by Nikolay Kanchevski on 11.12.26.
//

#include "WaylandCursorManager.h"

#include <cmath>

#include "WaylandWindow.h"

namespace Sierra
{

    struct WaylandCursorManager::Callbacks
    {
        static void relativePointerHandleRelativeMotion(void* userData, zwp_relative_pointer_v1*, uint32_t, uint32_t, const wl_fixed_t deltaX, const wl_fixed_t deltaY, wl_fixed_t, wl_fixed_t)
        {
            WaylandCursorManager& cursorManager = *static_cast<WaylandCursorManager*>(userData);
            if (cursorManager.lockedPointer == nullptr) return;

            // NOTE: Wayland uses -Y coordinates
            cursorManager.relativeMotion += Vector2(wl_fixed_to_double(deltaX), -wl_fixed_to_double(deltaY));
        }

        constexpr static zwp_relative_pointer_v1_listener RELATIVE_POINTER_LISTENER
        {
            .relative_motion = relativePointerHandleRelativeMotion
        };
    };

    /* --- CONSTRUCTORS --- */

    WaylandCursorManager::WaylandCursorManager(const WaylandContext& waylandContext, const WaylandWindow& window)
        : waylandContext(waylandContext), window(window) // TODO: ONE INSIDE ANOTHER
    {

    }

    /* --- POLLING METHODS --- */

    void WaylandCursorManager::RegisterCursorMove(const Vector2Int position)
    {
        cursorPosition = position;
        if (cursorShown && cursorPosition != lastCursorPosition) GetCursorMoveDispatcher().DispatchEvent(cursorPosition);
    }

    /* --- SETTER METHODS --- */

    void WaylandCursorManager::SetCursorVisibility(const bool visible)
    {
        if (visible == cursorShown) return;
        cursorShown = visible;

        // Wayland does not allow moving the cursor, so it cannot be kept within the window by re-centering it. Compositor is asked to lock it in place instead
        if (cursorShown) UnlockPointer();
        else LockPointer();

        ApplyCursor();
    }

    void WaylandCursorManager::SetCursorPosition(const Vector2Int position)
    {
        // NOTE: Wayland does not allow moving the cursor. The only exception is hinting where a locked one is to reappear, once it gets unlocked
        if (lockedPointer == nullptr) return;
        zwp_locked_pointer_v1_set_cursor_position_hint(lockedPointer, wl_fixed_from_int(position.x), wl_fixed_from_int(static_cast<int32>(window.GetHeight()) - position.y));

        // Reset mouse delta
        lastCursorPosition = position;
        cursorPosition = position;
    }

    /* --- GETTER METHODS --- */

    bool WaylandCursorManager::IsCursorVisible() const noexcept
    {
        return cursorShown;
    }

    Vector2Int WaylandCursorManager::GetCursorPosition() const noexcept
    {
        return cursorPosition;
    }

    Vector2 WaylandCursorManager::GetCursorDelta() const noexcept
    {
        if (lockedPointer != nullptr) return relativeMotion;
        return { cursorPosition.x - lastCursorPosition.x, cursorPosition.y - lastCursorPosition.y };
    }

    WindowingBackendType WaylandCursorManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Wayland;
    }

    /* --- DESTRUCTOR --- */

    WaylandCursorManager::~WaylandCursorManager() noexcept
    {
        UnlockPointer();
    }

    /* --- PRIVATE METHODS --- */

    void WaylandCursorManager::Update()
    {
        lastCursorPosition = cursorPosition;
        relativeMotion = { 0, 0 };
    }

    void WaylandCursorManager::RegisterCursorEnter(const Vector2Int position)
    {
        cursorInside = true;

        // Cursor has just appeared, so there is no motion to speak of yet
        cursorPosition = position;
        lastCursorPosition = position;

        // A hidden cursor may have been requested, while no pointer device was present to lock
        if (!cursorShown) LockPointer();
        ApplyCursor();
    }

    void WaylandCursorManager::RegisterCursorLeave()
    {
        cursorInside = false;
    }

    void WaylandCursorManager::ApplyCursor() const noexcept
    {
        // NOTE: Image of the cursor can only be changed, while it is over the window (it is otherwise managed by whatever it is over)
        if (!cursorInside) return;
        waylandContext.SetCursor(cursorShown, static_cast<uint32>(glm::ceil(window.GetScaling())));
    }

    void WaylandCursorManager::LockPointer()
    {
        if (lockedPointer != nullptr || waylandContext.GetPointer() == nullptr) return;

        // Without these, cursor is merely hidden, and its delta is still that of its absolute position
        if (waylandContext.GetPointerConstraints() == nullptr || waylandContext.GetRelativePointerManager() == nullptr) return;

        relativePointer = zwp_relative_pointer_manager_v1_get_relative_pointer(waylandContext.GetRelativePointerManager(), waylandContext.GetPointer());
        zwp_relative_pointer_v1_add_listener(relativePointer, &Callbacks::RELATIVE_POINTER_LISTENER, this);

        // NOTE: Lock is only in effect, while window has the pointer's focus, and persists across losing and regaining it
        lockedPointer = zwp_pointer_constraints_v1_lock_pointer(waylandContext.GetPointerConstraints(), window.GetSurface(), waylandContext.GetPointer(), nullptr, ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT);
        relativeMotion = { 0, 0 };
    }

    void WaylandCursorManager::UnlockPointer() noexcept
    {
        if (relativePointer != nullptr)
        {
            zwp_relative_pointer_v1_destroy(relativePointer);
            relativePointer = nullptr;
        }

        if (lockedPointer != nullptr)
        {
            zwp_locked_pointer_v1_destroy(lockedPointer);
            lockedPointer = nullptr;
        }

        relativeMotion = { 0, 0 };
    }

}
