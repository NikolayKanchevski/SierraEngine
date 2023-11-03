//
// Created by Nikolay Kanchevski on 10.23.23.
//

#include "X11InputManager.h"

#define Button6            (Button5 + 1)
#define Button7            (Button5 + 2)
#define Button8            (Button5 + 3)
#define Button9            (Button5 + 4)

namespace Sierra
{
    /* --- CONSTRUCTORS --- */

    X11InputManager::X11InputManager(const X11InputManagerCreateInfo &createInfo)
        : InputManager(createInfo), xkbExtension(createInfo.xkbExtension)
    {

    }

    /* --- POLLING METHODS --- */

    void X11InputManager::OnUpdate()
    {
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

    /* --- GETTER METHODS --- */

    bool X11InputManager::IsKeyPressed(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool X11InputManager::IsKeyHeld(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool X11InputManager::IsKeyReleased(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool X11InputManager::IsKeyResting(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool X11InputManager::IsMouseButtonPressed(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool X11InputManager::IsMouseButtonHeld(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool X11InputManager::IsMouseButtonReleased(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool X11InputManager::IsMouseButtonResting(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 X11InputManager::GetMouseScroll() const
    {
        return mouseScroll;
    }

    /* --- EVENTS --- */

    void X11InputManager::KeyPressEvent(const XEvent &event)
    {
        // Prevent out of bounds error
        if (event.xkey.keycode >= xkbExtension.GetKeyTable().size()) return;

        // Translate key
        const Key key = xkbExtension.GetKeyTable()[event.xkey.keycode];
        if (key == Key::Unknown) return;

        // Save key state and trigger events
        keyStates[GetKeyIndex(key)] = InputAction::Press;
        GetKeyPressDispatcher().DispatchEvent(key);
    }

    void X11InputManager::KeyReleaseEvent(const XEvent &event)
    {
        // Prevent out of bounds error
        if (event.xkey.keycode >= xkbExtension.GetKeyTable().size()) return;

        // Translate key
        const Key key = xkbExtension.GetKeyTable()[event.xkey.keycode];
        if (key == Key::Unknown) return;

        // Save key state and trigger events
        keyStates[GetKeyIndex(key)] = InputAction::Release;
        GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void X11InputManager::ButtonPressEvent(const XEvent &event)
    {
        switch (event.xbutton.button)
        {
            case Button1:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Left;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button2:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Middle;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button3:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Right;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button4:
            {
                // Save scrolling inertia and trigger events
                mouseScroll.y = 1.0f;
                GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
                break;
            }
            case Button5:
            {
                // Save scrolling inertia and trigger events
                mouseScroll.y = -1.0f;
                GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
                break;
            }
            case Button6:
            {
                // Save scrolling inertia and trigger events
                mouseScroll.x = 1.0f;
                GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
                break;
            }
            case Button7:
            {
                // Save scrolling inertia and trigger events
                mouseScroll.x = -1.0f;
                GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
                break;
            }
            case Button8:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Extra1;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button9:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Extra2;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
                GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void X11InputManager::ButtonReleaseEvent(const XEvent &event)
    {
        switch (event.xbutton.button)
        {
            case Button1:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Left;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button2:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Middle;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button3:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Right;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button8:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Extra1;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            case Button9:
            {
                // Save mouse button state and trigger events
                const MouseButton mouseButton = MouseButton::Extra2;
                mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
                GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
                break;
            }
            default:
            {
                break;
            }
        }
    }

}