//
// Created by Nikolay Kanchevski on 10.17.2023.
//

#include "Win32InputManager.h"

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
    #define NOMINMAX
#endif
#include <Windows.h>

namespace Sierra
{

    /* --- CONVERSIONS --- */

    Key VirtualKeyCodeToKey(const uint32 virtualKeyCode) noexcept
    {
        /* === Reference: https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes === */
        if (virtualKeyCode >= 'A' && virtualKeyCode <= 'Z') return static_cast<Key>(static_cast<uint32>(Key::A) + (virtualKeyCode - 'A'));
        if (virtualKeyCode >= '0' && virtualKeyCode <= '9') return static_cast<Key>(static_cast<uint32>(Key::Number0) + (virtualKeyCode - '0'));
        if (virtualKeyCode >= VK_NUMPAD0 && virtualKeyCode <= VK_NUMPAD9) return static_cast<Key>(static_cast<uint32>(Key::KeypadNumber0) + (virtualKeyCode - VK_NUMPAD0));
        if (virtualKeyCode >= VK_F1 && virtualKeyCode <= VK_F24) return static_cast<Key>(static_cast<uint32>(Key::F1) + (virtualKeyCode - VK_F1));

        switch (virtualKeyCode)
        {
            case VK_SPACE:          return Key::Space;
            case VK_ESCAPE:         return Key::Escape;
            case VK_OEM_MINUS:      return Key::Minus;
            case VK_OEM_PLUS:       return Key::Equal;
            case VK_OEM_COMMA:      return Key::Comma;
            case VK_OEM_PERIOD:     return Key::Period;
            case VK_OEM_7:          return Key::Apostrophe;
            case VK_OEM_1:          return Key::Semicolon;
            case VK_OEM_2:          return Key::Slash;
            case VK_OEM_5:          return Key::Backslash;
            case VK_OEM_4:          return Key::LeftBracket;
            case VK_OEM_6:          return Key::RightBracket;
            case VK_OEM_3:          return Key::Grave;
            case VK_OEM_8:          return Key::World1;
            case VK_OEM_102:        return Key::World2;
            case VK_RETURN:         return Key::Enter;
            case VK_BACK:           return Key::Backspace;
            case VK_DELETE:         return Key::Delete;
            case VK_TAB:            return Key::Tab;
            case VK_CAPITAL:        return Key::CapsLock;
            case VK_INSERT:         return Key::Insert;
            case VK_RIGHT:          return Key::RightArrow;
            case VK_LEFT:           return Key::LeftArrow;
            case VK_DOWN:           return Key::DownArrow;
            case VK_UP:             return Key::UpArrow;
            case VK_PRIOR:          return Key::PageUp;
            case VK_NEXT:           return Key::PageDown;
            case VK_HOME:           return Key::Home;
            case VK_END:            return Key::End;
            case VK_SCROLL:         return Key::ScrollLock;
            case VK_NUMLOCK:        return Key::NumpadLock;
            case VK_SNAPSHOT:       return Key::PrintScreen;
            case VK_PAUSE:          return Key::Pause;
            case VK_DECIMAL:        return Key::KeypadPeriod;
            case VK_DIVIDE:         return Key::KeypadSlash;
            case VK_MULTIPLY:       return Key::KeypadAsterisk;
            case VK_SUBTRACT:       return Key::KeypadMinus;
            case VK_ADD:            return Key::KeypadPlus;
            case VK_SHIFT:
            case VK_LSHIFT:         return Key::LeftShift;
            case VK_RSHIFT:         return Key::RightShift;
            case VK_CONTROL:
            case VK_LCONTROL:       return Key::LeftControl;
            case VK_RCONTROL:       return Key::RightControl;
            case VK_MENU:
            case VK_LMENU:          return Key::LeftAlt;
            case VK_RMENU:          return Key::RightAlt;
            case VK_LWIN:           return Key::LeftSystem;
            case VK_RWIN:           return Key::RightSystem;
            case VK_APPS:           return Key::Menu;
            default:                break;
        }

        return Key::Unknown;
    }

    /* --- POLLING METHODS --- */

    void Win32InputManager::RegisterKeyPress(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Press;
        GetKeyPressDispatcher().DispatchEvent(key);
    }

    void Win32InputManager::RegisterKeyRelease(const Key key)
    {
        if (key == Key::Unknown) return;
        keyStates[GetKeyIndex(key)] = InputAction::Release;
        GetKeyReleaseDispatcher().DispatchEvent(key);
    }

    void Win32InputManager::RegisterMouseButtonPress(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Press;
        GetMouseButtonPressDispatcher().DispatchEvent(mouseButton);
    }

    void Win32InputManager::RegisterMouseButtonRelease(const MouseButton mouseButton)
    {
        if (mouseButton == MouseButton::Unknown) return;
        mouseButtonStates[GetMouseButtonIndex(mouseButton)] = InputAction::Release;
        GetMouseButtonReleaseDispatcher().DispatchEvent(mouseButton);
    }

    void Win32InputManager::RegisterMouseScroll(const Vector2 scroll)
    {
        // Several scroll steps may happen between two updates (such as with high-resolution wheels and touchpads), so they are accumulated
        mouseScroll += scroll;
        GetMouseScrollDispatcher().DispatchEvent(scroll);
    }

    /* --- GETTER METHODS --- */

    bool Win32InputManager::IsKeyPressed(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyHeld(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsKeyReleased(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Press && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsKeyResting(const Key key) const noexcept
    {
        return lastKeyStates[GetKeyIndex(key)] == InputAction::Release && keyStates[GetKeyIndex(key)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonPressed(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonHeld(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press;
    }

    bool Win32InputManager::IsMouseButtonReleased(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Press && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    bool Win32InputManager::IsMouseButtonResting(const MouseButton mouseButton) const noexcept
    {
        return lastMouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release && mouseButtonStates[GetMouseButtonIndex(mouseButton)] == InputAction::Release;
    }

    Vector2 Win32InputManager::GetMouseScroll() const noexcept
    {
        return mouseScroll;
    }

    WindowingBackendType Win32InputManager::GetBackendType() const noexcept
    {
        return WindowingBackendType::Win32;
    }

    /* --- PRIVATE METHODS --- */

    void Win32InputManager::Update()
    {
        std::ranges::copy(keyStates, lastKeyStates.begin());
        std::ranges::copy(mouseButtonStates, lastMouseButtonStates.begin());
        mouseScroll = { 0, 0 };
    }

    void Win32InputManager::ReleaseKeys()
    {
        for (uint32 i = 0; i < KEY_COUNT; i++)
        {
            if (keyStates[i] == InputAction::Press) RegisterKeyRelease(static_cast<Key>(i));
        }
    }

    void Win32InputManager::ReleaseMouseButtons()
    {
        for (uint32 i = 0; i < MOUSE_BUTTON_COUNT; i++)
        {
            if (mouseButtonStates[i] == InputAction::Press) RegisterMouseButtonRelease(static_cast<MouseButton>(i));
        }
    }

    bool Win32InputManager::IsKeyDown(const Key key) const noexcept
    {
        return keyStates[GetKeyIndex(key)] == InputAction::Press;
    }

    bool Win32InputManager::IsAnyMouseButtonDown() const noexcept
    {
        return std::ranges::any_of(mouseButtonStates, [](const InputAction action) -> bool { return action == InputAction::Press; });
    }

}
