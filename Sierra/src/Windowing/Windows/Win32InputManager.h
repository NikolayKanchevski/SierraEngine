//
// Created by Nikolay Kanchevski on 10.17.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32InputManager.h file is only allowed in Windows builds!"
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../InputManager.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API Key VirtualKeyCodeToKey(UINT keyCode);
    [[nodiscard]] SIERRA_API MouseButton VirtualKeyCodeToMouseButton(UINT keyCode);

    class SIERRA_API Win32InputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        Win32InputManager() = default;

        /* --- POLLING METHODS --- */
        void RegisterKeyPress(Key key) override;
        void RegisterKeyRelease(Key key) override;

        void RegisterMouseButtonPress(MouseButton mouseButton) override;
        void RegisterMouseButtonRelease(MouseButton mouseButton) override;
        void RegisterMouseScroll(Vector2 scroll) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsKeyPressed(Key key) const noexcept override;
        [[nodiscard]] bool IsKeyHeld(Key key) const noexcept override;
        [[nodiscard]] bool IsKeyReleased(Key key) const noexcept override;
        [[nodiscard]] bool IsKeyResting(Key key) const noexcept override;

        [[nodiscard]] bool IsMouseButtonPressed(MouseButton mouseButton) const noexcept override;
        [[nodiscard]] bool IsMouseButtonHeld(MouseButton mouseButton) const noexcept override;
        [[nodiscard]] bool IsMouseButtonReleased(MouseButton mouseButton) const noexcept override;
        [[nodiscard]] bool IsMouseButtonResting(MouseButton mouseButton) const noexcept override;
        [[nodiscard]] Vector2 GetMouseScroll() const noexcept override;

        [[nodiscard]] WindowingBackendType GetBackendType() const noexcept override;

        /* --- COPY SEMANTICS --- */
        Win32InputManager(const Win32InputManager&) = delete;
        Win32InputManager& operator=(const Win32InputManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32InputManager(Win32InputManager&&) = delete;
        Win32InputManager& operator=(Win32InputManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32InputManager() noexcept override = default;

    private:
        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };

        Vector2 mouseScroll = { 0, 0 };

        friend class Win32Window;
        void Update();

    };

}
