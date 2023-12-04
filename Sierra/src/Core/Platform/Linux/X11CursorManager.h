//
// Created by Nikolay Kanchevski on 10.28.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11CursorManager.h file is only allowed in Linux builds!"
#endif

#include "../../CursorManager.h"

#include <X11/Xlib.h>
#include "X11Context.h"

namespace Sierra
{

    struct X11CursorManagerCreateInfo final : public CursorManagerCreateInfo
    {
        const XID window;
        const X11Context &x11Context;
    };

    class SIERRA_API X11CursorManager final : public CursorManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11CursorManager(const X11CursorManagerCreateInfo &createInfo);

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
        const XID window;
        const X11Context &x11Context;

        Vector2Int cursorPosition = { 0, 0 };
        Vector2Int lastCursorPosition = { 0, 0 };
        bool cursorHidden = false;
        bool justHidCursor = false;

        friend class X11Window;
        void OnWindowInitialize();
        void OnUpdate();
        void OnUpdateEnd();
        void MotionNotifyEvent(const XEvent &event);

    };

}
