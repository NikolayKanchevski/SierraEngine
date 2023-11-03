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

        /* --- POLLING METHODS --- */
        void OnWindowInitialize();
        void OnUpdate();
        void OnUpdateEnd();

        /* --- SETTER METHODS --- */
        void SetCursorPosition(const Vector2 &position) override;
        void ShowCursor() override;
        void HideCursor() override;

        /* --- GETTER METHODS --- */
        Vector2 GetCursorPosition() override;
        float32 GetHorizontalDelta() override;
        float32 GetVerticalDelta() override;
        bool IsCursorHidden() override;

        /* --- EVENTS --- */
        void MotionNotifyEvent(const XEvent &event);

    private:
        const XID window;
        const X11Context &x11Context;

        Vector2Int cursorPosition = { 0, 0 };
        Vector2Int lastCursorPosition = { 0, 0 };
        bool cursorHidden = false;
        bool justHidCursor = false;

    };

}
