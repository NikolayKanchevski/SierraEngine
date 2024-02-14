//
// Created by Nikolay Kanchevski on 10.23.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11InputManager.h file is only allowed in Linux builds!"
#endif

#include "../../InputManager.h"

#include <X11/Xlib.h>
#include "X11Extensions.h"

namespace Sierra
{

    class SIERRA_API X11InputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11InputManager(const XkbExtension &xkbExtension, const InputManagerCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        bool IsKeyPressed(Key key) const override;
        bool IsKeyHeld(Key key) const override;
        bool IsKeyReleased(Key key) const override;
        bool IsKeyResting(Key key) const override;

        bool IsMouseButtonPressed(MouseButton mouseButton) const override;
        bool IsMouseButtonHeld(MouseButton mouseButton) const override;
        bool IsMouseButtonReleased(MouseButton mouseButton) const override;
        bool IsMouseButtonResting(MouseButton mouseButton) const override;
        Vector2 GetMouseScroll() const override;

    private:
        const XkbExtension &xkbExtension;

        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };

        Vector2 mouseScroll = { 0, 0 };

        friend class X11Window;
        void Update();
        void KeyPressEvent(const XEvent &event);
        void KeyReleaseEvent(const XEvent &event);
        void ButtonPressEvent(const XEvent &event);
        void ButtonReleaseEvent(const XEvent &event);

    };

}