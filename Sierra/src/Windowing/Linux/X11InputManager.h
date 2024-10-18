//
// Created by Nikolay Kanchevski on 10.23.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the X11InputManager.h file is only allowed in Linux builds!"
#endif

#include "../InputManager.h"

#include "../../Platform/Linux/X11Extensions.h"

namespace Sierra
{

    class SIERRA_API X11InputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit X11InputManager(const XkbExtension& xkbExtension);

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

        /* --- CONVERSIONS --- */
        [[nodiscard]] Key XKeyCodeToKey(uint keyCode);
        [[nodiscard]] MouseButton XButtonToMouseButton(uint button);

        /* --- COPY SEMANTICS --- */
        X11InputManager(const X11InputManager&) = delete;
        X11InputManager& operator=(const X11InputManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        X11InputManager(X11InputManager&&) = delete;
        X11InputManager& operator=(X11InputManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~X11InputManager() noexcept override = default;

    private:
        const XkbExtension& xkbExtension;

        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };

        Vector2 mouseScroll = { 0, 0 };

        friend class X11Window;
        void Update();

    };

}