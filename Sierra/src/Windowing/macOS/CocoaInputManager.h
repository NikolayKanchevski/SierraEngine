//
// Created by Nikolay Kanchevski on 19.09.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the CocoaInputManager.h file is only allowed in macOS builds!"
#endif

#include "../InputManager.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API Key KeyCodeToKey(uint32 keyCode) noexcept;
    [[nodiscard]] SIERRA_API MouseButton ButtonNumberToMouseButton(uint32 buttonNumber) noexcept;

    class SIERRA_API CocoaInputManager final : public InputManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        CocoaInputManager();

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
        CocoaInputManager(const CocoaInputManager&) = delete;
        CocoaInputManager& operator=(const CocoaInputManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        CocoaInputManager(CocoaInputManager&&) = delete;
        CocoaInputManager& operator=(CocoaInputManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~CocoaInputManager() noexcept override = default;

    private:
        std::array<InputAction, KEY_COUNT> lastKeyStates { };
        std::array<InputAction, KEY_COUNT> keyStates { };

        std::array<InputAction, MOUSE_BUTTON_COUNT> lastMouseButtonStates { };
        std::array<InputAction, MOUSE_BUTTON_COUNT> mouseButtonStates { };
        Vector2 mouseScroll = { 0.0f, 0.0f };

        friend class CocoaWindow;
        void Update();

    };

}
