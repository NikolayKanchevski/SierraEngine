//
// Created by Nikolay Kanchevski on 19.09.23.
//

#include "CocoaInputManager.h"

#define GET_KEY_INDEX(KEY) static_cast<std::underlying_type<Key>::type>(KEY)
#define GET_MOUSE_BUTTON_INDEX(MOUSE_BUTTON) static_cast<std::underlying_type<MouseButton>::type>(MOUSE_BUTTON)

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CocoaInputManager::CocoaInputManager(const InputManagerCreateInfo &createInfo)
        : InputManager(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void CocoaInputManager::OnUpdate()
    {
        memcpy(lastKeyStates, keyStates, sizeof(keyStates) / sizeof(keyStates[0]));
        memcpy(lastMouseButtonStates, mouseButtonStates, sizeof(mouseButtonStates) / sizeof(mouseButtonStates[0]));
        mouseScroll = { 0, 0 };
    }

    /* --- GETTER METHODS --- */

    bool CocoaInputManager::IsKeyPressed(const Key key)
    {
        return lastKeyStates[GET_KEY_INDEX(key)] == InputAction::Release && keyStates[GET_KEY_INDEX(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyHeld(const Key key)
    {
        return lastKeyStates[GET_KEY_INDEX(key)] == InputAction::Press && keyStates[GET_KEY_INDEX(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyReleased(const Key key)
    {
        return lastKeyStates[GET_KEY_INDEX(key)] == InputAction::Press && keyStates[GET_KEY_INDEX(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsKeyResting(const Key key)
    {
        return lastKeyStates[GET_KEY_INDEX(key)] == InputAction::Release && keyStates[GET_KEY_INDEX(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonPressed(const MouseButton mouseButton)
    {
        return lastMouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Release && mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonHeld(const MouseButton mouseButton)
    {
        return lastMouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Press && mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonReleased(const MouseButton mouseButton)
    {
        return lastMouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Press && mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonResting(const MouseButton mouseButton)
    {
        return lastMouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Release && mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] == InputAction::Release;
    }

    Vector2 CocoaInputManager::GetMouseScroll()
    {
        return mouseScroll;
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void CocoaInputManager::KeyDown(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event keyCode] > (sizeof(KEY_TABLE) / sizeof(KEY_TABLE[0]))) return;

            // Translate key
            const Key key = KEY_TABLE[[event keyCode]];
            if (key == Key::Unknown) return;

            // Translate modifier flags
            KeyModifierFlags modifierFlags = KEY_MODIFIER_FLAGS_NONE;
            if ([event modifierFlags] & NSEventModifierFlagShift)    modifierFlags |= KEY_MODIFIER_FLAGS_SHIFT;
            if ([event modifierFlags] & NSEventModifierFlagControl)  modifierFlags |= KEY_MODIFIER_FLAGS_CONTROL;
            if ([event modifierFlags] & NSEventModifierFlagOption)   modifierFlags |= KEY_MODIFIER_FLAGS_ALT;
            if ([event modifierFlags] & NSEventModifierFlagCommand)  modifierFlags |= KEY_MODIFIER_FLAGS_SYSTEM;
            if ([event modifierFlags] & NSEventModifierFlagCapsLock) modifierFlags |= KEY_MODIFIER_FLAGS_CAPS_LOCK;

            // Save key state and trigger events
            keyStates[GET_KEY_INDEX(key)] = InputAction::Press;
            GetKeyPressDispatcher().DispatchEvent(key);
        }

        void CocoaInputManager::FlagsChanged(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event keyCode] > (sizeof(KEY_TABLE) / sizeof(KEY_TABLE[0]))) return;

            // Translate key
            const Key key = KEY_TABLE[[event keyCode]];
            if (key == Key::Unknown) return;

            // Translate modifier flags
            KeyModifierFlags modifierFlags = KEY_MODIFIER_FLAGS_NONE;
            if ([event modifierFlags] & NSEventModifierFlagShift)    modifierFlags |= KEY_MODIFIER_FLAGS_SHIFT;
            if ([event modifierFlags] & NSEventModifierFlagControl)  modifierFlags |= KEY_MODIFIER_FLAGS_CONTROL;
            if ([event modifierFlags] & NSEventModifierFlagOption)   modifierFlags |= KEY_MODIFIER_FLAGS_ALT;
            if ([event modifierFlags] & NSEventModifierFlagCommand)  modifierFlags |= KEY_MODIFIER_FLAGS_SYSTEM;
            if ([event modifierFlags] & NSEventModifierFlagCapsLock) modifierFlags |= KEY_MODIFIER_FLAGS_CAPS_LOCK;

            // Modifier keys need to be registered as pressed manually, so we get modifier flags of the modifier key... fun stuff!
            NSEventModifierFlags nsModifierFlags = 0;
            switch (key)
            {
                case Key::LeftShift:
                case Key::RightShift:
                {
                    nsModifierFlags = NSEventModifierFlagShift;
                    break;
                }
                case Key::LeftControl:
                case Key::RightControl:
                {
                    nsModifierFlags = NSEventModifierFlagControl;
                    break;
                }
                case Key::LeftOption:
                case Key::RightOption:
                {
                    nsModifierFlags = NSEventModifierFlagOption;
                    break;
                }
                case Key::LeftCommand:
                case Key::RightCommand:
                {
                    nsModifierFlags = NSEventModifierFlagCommand;
                    break;
                }
                case Key::CapsLock:
                {
                    nsModifierFlags = NSEventModifierFlagCapsLock;
                    break;
                }
                default:
                {
                    break;
                }
            }

            // Determine key action
            InputAction action;
            if (nsModifierFlags & ([event modifierFlags] & NSEventModifierFlagDeviceIndependentFlagsMask))
            {
                action = keyStates[GET_KEY_INDEX(key)] == InputAction::Press ? InputAction::Release : InputAction::Press;
            }
            else
            {
                action = InputAction::Release;
            }

            // Save key state and trigger events
            keyStates[GET_KEY_INDEX(key)] = action;
            if (action == InputAction::Press) GetKeyPressDispatcher().DispatchEvent(key);
            else if (action == InputAction::Release) GetKeyReleaseDispatcher().DispatchEvent(key);
        }

        void CocoaInputManager::KeyUp(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event keyCode] > (sizeof(KEY_TABLE) / sizeof(KEY_TABLE[0]))) return;

            // Translate key
            const Key key = KEY_TABLE[[event keyCode]];
            if (key == Key::Unknown) return;

            // Translate modifier flags
            KeyModifierFlags modifierFlags = KEY_MODIFIER_FLAGS_NONE;
            if ([event modifierFlags] & NSEventModifierFlagShift)    modifierFlags |= KEY_MODIFIER_FLAGS_SHIFT;
            if ([event modifierFlags] & NSEventModifierFlagControl)  modifierFlags |= KEY_MODIFIER_FLAGS_CONTROL;
            if ([event modifierFlags] & NSEventModifierFlagOption)   modifierFlags |= KEY_MODIFIER_FLAGS_ALT;
            if ([event modifierFlags] & NSEventModifierFlagCommand)  modifierFlags |= KEY_MODIFIER_FLAGS_SYSTEM;
            if ([event modifierFlags] & NSEventModifierFlagCapsLock) modifierFlags |= KEY_MODIFIER_FLAGS_CAPS_LOCK;

            // Save key state and trigger events
            keyStates[GET_KEY_INDEX(key)] = InputAction::Release;
            GetKeyReleaseDispatcher().DispatchEvent(key);
        }

        void CocoaInputManager::MouseDown(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Left;

            // Save mouse button state and trigger events
            mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] = InputAction::Press;
            GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::RightMouseDown(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Right;

            // Save mouse button state and trigger events
            mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] = InputAction::Press;
            GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::OtherMouseDown(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event buttonNumber] > GET_MOUSE_BUTTON_INDEX(MouseButton::Extra2)) return;

            // Save mouse button
            const MouseButton mouseButton = static_cast<MouseButton>([event buttonNumber] + 1);

            // Save mouse button state and trigger events
            mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] = InputAction::Press;
            GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::MouseUp(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Left;

            // Save mouse button state and trigger events
            mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] = InputAction::Release;
            GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::RightMouseUp(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Right;

            // Save mouse button state and trigger events
            mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] = InputAction::Release;
            GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::OtherMouseUp(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event buttonNumber] > GET_MOUSE_BUTTON_INDEX(MouseButton::Extra2)) return;

            // Save mouse button
            const MouseButton mouseButton = static_cast<MouseButton>([event buttonNumber] + 1);

            // Save mouse button state and trigger events
            mouseButtonStates[GET_MOUSE_BUTTON_INDEX(mouseButton)] = InputAction::Release;
            GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::ScrollWheel(const NSEvent* event)
        {
            mouseScroll = { [event deltaX], [event scrollingDeltaY] };
            if ([event hasPreciseScrollingDeltas])
            {
                mouseScroll *= 0.1;
            }

            GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
        }
    #endif

}