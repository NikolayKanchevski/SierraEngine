//
// Created by Nikolay Kanchevski on 19.09.23.
//

#include "CocoaInputManager.h"

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
        std::copy(keyStates.begin(), keyStates.end(), lastKeyStates.begin());
        std::copy(mouseButtonStates.begin(), mouseButtonStates.end(), lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

    /* --- GETTER METHODS --- */

    bool CocoaInputManager::IsKeyPressed(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyHeld(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool CocoaInputManager::IsKeyReleased(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsKeyResting(const Key key) const
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonPressed(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonHeld(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool CocoaInputManager::IsMouseButtonReleased(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool CocoaInputManager::IsMouseButtonResting(const MouseButton mouseButton) const
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 CocoaInputManager::GetMouseScroll() const
    {
        return mouseScroll;
    }

    /* --- EVENTS --- */

    #if defined(__OBJC__)
        void CocoaInputManager::KeyDown(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event keyCode] >= KEY_TABLE.size()) return;

            // Translate key
            const Key key = KEY_TABLE[[event keyCode]];
            if (key == Key::Unknown) return;

            // Save key state and trigger events
            keyStates[GetKeyIndex(key)] = InputAction::Press;
            GetKeyPressDispatcher().DispatchEvent(key);
        }

        void CocoaInputManager::FlagsChanged(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event keyCode] >= KEY_TABLE.size()) return;

            // Translate key
            const Key key = KEY_TABLE[[event keyCode]];
            if (key == Key::Unknown) return;

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
                action = keyStates[GetKeyIndex(key)] == InputAction::Press ? InputAction::Release : InputAction::Press;
            }
            else
            {
                action = InputAction::Release;
            }

            // Save key state and trigger events
            keyStates[GetKeyIndex(key)] = action;
            if (action == InputAction::Press) GetKeyPressDispatcher().DispatchEvent(key);
            else if (action == InputAction::Release) GetKeyReleaseDispatcher().DispatchEvent(key);
        }

        void CocoaInputManager::KeyUp(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event keyCode] >= KEY_TABLE.size()) return;

            // Translate key
            const Key key = KEY_TABLE[[event keyCode]];
            if (key == Key::Unknown) return;

            // Save key state and trigger events
            keyStates[GetKeyIndex(key)] = InputAction::Release;
            GetKeyReleaseDispatcher().DispatchEvent(key);
        }

        void CocoaInputManager::MouseDown(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Left;

            // Save mouse button state and trigger events
            mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
            GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::RightMouseDown(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Right;

            // Save mouse button state and trigger events
            mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
            GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::OtherMouseDown(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event buttonNumber] > GetMouseButtonIndex(MouseButton::Extra2)) return;

            // Save mouse button
            const MouseButton mouseButton = static_cast<MouseButton>([event buttonNumber] + 1);

            // Save mouse button state and trigger events
            mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
            GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::MouseUp(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Left;

            // Save mouse button state and trigger events
            mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
            GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::RightMouseUp(const NSEvent* event)
        {
            // Save mouse button
            const MouseButton mouseButton = MouseButton::Right;

            // Save mouse button state and trigger events
            mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
            GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::OtherMouseUp(const NSEvent* event)
        {
            // Prevent out of bounds error
            if ([event buttonNumber] > GetMouseButtonIndex(MouseButton::Extra2)) return;

            // Save mouse button
            const MouseButton mouseButton = static_cast<MouseButton>([event buttonNumber] + 1);

            // Save mouse button state and trigger events
            mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
            GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
        }

        void CocoaInputManager::ScrollWheel(const NSEvent* event)
        {
            // Save scroll inertia
            mouseScroll = { [event deltaX], [event scrollingDeltaY] };

            // Trigger events
            GetMouseScrollDispatcher().DispatchEvent(mouseScroll);
        }
    #endif

}