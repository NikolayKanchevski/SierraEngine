//
// Created by Nikolay Kanchevski on 11.12.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the WaylandInputManager.h file is only allowed in Linux builds!"
#endif

#include "../../InputManager.h"

namespace Sierra
{

    class SIERRA_API WaylandInputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        WaylandInputManager() noexcept = default;

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
        [[nodiscard]] static Key ScancodeToKey(uint32 scancode) noexcept;
        [[nodiscard]] static MouseButton ButtonCodeToMouseButton(uint32 buttonCode) noexcept;

        /* --- COPY SEMANTICS --- */
        WaylandInputManager(const WaylandInputManager&) = delete;
        WaylandInputManager& operator=(const WaylandInputManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        WaylandInputManager(WaylandInputManager&&) = delete;
        WaylandInputManager& operator=(WaylandInputManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WaylandInputManager() noexcept override = default;

    private:
        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };

        Vector2 mouseScroll = { 0, 0 };

        friend class WaylandWindow;
        void Update();
        void ReleaseKeys();
        void ReleaseMouseButtons();

    };

}
