//
// Created by Nikolay Kanchevski on 29.09.22.
//

#pragma once

/// @brief Contains useful methods for keyboard, mouse, and gamepad input querying.
namespace Sierra::Engine::Input
{

    /* --- PROPERTIES --- */
    constexpr uint MAX_GAME_PADS = 8;

    /* --- POLLING METHODS --- */
    void Start();
    void Update();

    /* --- SETTER METHODS --- */
    void SetGamePadMinimumStickSensitivity(float minimumSensitivity, uint player = 0);
    void SetGamePadMinimumLeftStickSensitivity(float minimumSensitivity, uint player = 0);
    void SetGamePadMinimumRightStickSensitivity(float minimumSensitivity, uint player = 0);

    /* --- GETTER METHODS --- */
    /// @brief Checks whether a key on the keyboard is pressed.
    /// @param keyCode The key to check.
    [[nodiscard]] bool GetKeyPressed(Key key);

    /// @brief Checks whether a key on the keyboard is held. Also returns true if pressed.
    /// @param keyCode The key to check.
    [[nodiscard]] bool GetKeyHeld(Key key);

    /// @brief Checks whether a key on the keyboard is released.
    /// @param keyCode The key to check.
    [[nodiscard]] bool GetKeyReleased(Key key);

    /// @brief Checks whether a key on the keyboard is resting - is not pressed, held, or released.
    /// @param keyCode The key to check.
    [[nodiscard]] bool GetKeyResting(Key key);

    /// @brief Returns the currently inputted from the keyboard characters.
    [[nodiscard]] std::vector<String> *GetEnteredCharacters();

    /// @brief Checks whether a button on the mouse is pressed.
    /// @param buttonCode The button to check.
    [[nodiscard]] bool GetMouseButtonPressed(MouseButton button);

    /// @brief Checks whether a button on the mouse is held. Also returns true if pressed.
    /// @param buttonCode The button to check.
    [[nodiscard]] bool GetMouseButtonHeld(MouseButton button);

    /// @brief Checks whether a button on the mouse is released.
    /// @param buttonCode The button to check.
    [[nodiscard]] bool GetMouseButtonReleased(MouseButton button);

    /// @brief Checks whether a button on the mouse is resting - is not pressed, held, or released.
    /// @param buttonCode The button to check.
    [[nodiscard]] bool GetMouseButtonResting(MouseButton button);

    /// @brief Returns the horizontal mouse scroll delta. Horizontal scrolling is usually only supported on laptops,
    /// or special external input devices.
    [[nodiscard]] float GetHorizontalMouseScroll();

    /// @brief Returns the vertical mouse scroll delta.
    [[nodiscard]] float GetVerticalMouseScroll();

    /// @brief Gets the model tag of a given player's game pad.
    [[nodiscard]] String GetGamePadName(uint player = 0);

    /// @brief Checks whether a button on a given gamepad is pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonPressed(GamePadButton button, uint player = 0);

    /// @brief Checks whether a button on a given gamepad is held. Also returns true if pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonHeld(GamePadButton button, uint player = 0);

    /// @brief Checks whether a button on a given gamepad is released.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonReleased(GamePadButton button, uint player = 0);

    /// @brief Checks whether a button on a given gamepad is resting - is not pressed, held, or released.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonResting(GamePadButton button, uint player = 0);

    /// @brief Returns a Vector2 where X is the horizontal axis of the left stick (LSB) and Y the vertical.
    /// See <see cref="GetHorizontalGamepadLeftStickAxis"/> and/or <see cref="GetVerticalGamepadLeftStickAxis"/>
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] Vector2 GetGamePadLeftStickAxis(uint player = 0);

    /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the left (LSB) stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] Vector2 GetGamePadRightStickAxis(uint player = 0);


    /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of the left (LSB) stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetHorizontalGamePadLeftStickAxis(uint player = 0);

    /// @brief Returns a Vector2 where X is the horizontal axis of the left right (RSB) and Y the vertical.
    /// See <see cref="GetHorizontalGamepadRightStickAxis"/> and/or <see cref="GetVerticalGamepadRightStickAxis"/>
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetVerticalGamePadLeftStickAxis(uint player = 0);

    /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the right (RSB) stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetHorizontalGamePadRightStickAxis(uint player = 0);

    /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of right (RSB) the stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetVerticalGamePadRightStickAxis(uint player = 0);

    /// @brief Returns a value between -1 and 1 indicating how hard the left trigger (LT button) is pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetGamePadLeftTriggerAxis(uint player = 0);

    /// @brief Returns a value between -1 and 1 indicating how hard the right trigger (RT button) is pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetGamePadRightTriggerAxis(uint player = 0);

    /// @brief Checks whether a given gamepad is connected and detected.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadConnected(uint player = 0);

    /* --- CALLBACKS --- */
    void KeyboardCharacterCallback(GLFWwindow *windowPtr, uint character);
    void KeyboardKeyCallback(GLFWwindow* windowPtr, int keyCode, int scanCode, int action, int mods);
    void MouseButtonCallback(GLFWwindow* windowPtr, int buttonCode, int action, int mods);
    void MouseScrollCallback(GLFWwindow* windowPtr, double xScroll, double yScroll);
    void JoystickCallback(int joystickID, int event);

}