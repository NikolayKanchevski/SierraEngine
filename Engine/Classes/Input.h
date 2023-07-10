//
// Created by Nikolay Kanchevski on 29.09.22.
//

#pragma once

namespace Sierra::Engine
{

    /// @brief Contains useful methods for keyboard, mouse, and gamepad input querying.
    class Input
    {
    public:
        /* --- PROPERTIES --- */
        static constexpr uint MAX_GAME_PADS = 8;

        /* --- POLLING METHODS --- */
        static void Start();
        static void Update();

        /* --- SETTER METHODS --- */
        static void SetGamePadMinimumStickSensitivity(float minimumSensitivity, uint player = 0);
        static void SetGamePadMinimumLeftStickSensitivity(float minimumSensitivity, uint player = 0);
        static void SetGamePadMinimumRightStickSensitivity(float minimumSensitivity, uint player = 0);

        /* --- GETTER METHODS --- */
        /// @brief Checks whether a key on the keyboard is pressed.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyPressed(Key keycode);

        /// @brief Checks whether a key on the keyboard is held. Also returns true if pressed.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyHeld(Key keycode);

        /// @brief Checks whether a key on the keyboard is released.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyReleased(Key keycode);

        /// @brief Checks whether a key on the keyboard is resting - is not pressed, held, or released.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyResting(Key keycode);

        /// @brief Returns the currently inputted from the keyboard characters.
        [[nodiscard]] static std::vector<String> *GetEnteredCharacters();

        /// @brief Checks whether a button on the mouse is pressed.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonPressed(uint buttonCode);

        /// @brief Checks whether a button on the mouse is held. Also returns true if pressed.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonHeld(uint buttonCode);

        /// @brief Checks whether a button on the mouse is released.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonReleased(uint buttonCode);

        /// @brief Checks whether a button on the mouse is resting - is not pressed, held, or released.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonResting(uint buttonCode);

        /// @brief Returns the horizontal mouse scroll delta. Horizontal scrolling is usually only supported on laptops,
        /// or special external input devices.
        [[nodiscard]] static float GetHorizontalMouseScroll();

        /// @brief Returns the vertical mouse scroll delta.
        [[nodiscard]] static float GetVerticalMouseScroll();

        /// @brief Gets the model tag of a given player's game pad.
        [[nodiscard]] static String GetGamePadName(uint player = 0);

        /// @brief Checks whether a button on a given gamepad is pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonPressed(uint gamePadButton, uint player = 0);

        /// @brief Checks whether a button on a given gamepad is held. Also returns true if pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonHeld(uint gamePadButton, uint player = 0);

        /// @brief Checks whether a button on a given gamepad is released.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonReleased(uint gamePadButton, uint player = 0);

        /// @brief Checks whether a button on a given gamepad is resting - is not pressed, held, or released.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonResting(uint gamePadButton, uint player = 0);

        /// @brief Returns a Vector2 where X is the horizontal axis of the left stick (LSB) and Y the vertical.
        /// See <see cref="GetHorizontalGamepadLeftStickAxis"/> and/or <see cref="GetVerticalGamepadLeftStickAxis"/>
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static Vector2 GetGamePadLeftStickAxis(uint player = 0);

        /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the left (LSB) stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static Vector2 GetGamePadRightStickAxis(uint player = 0);


        /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of the left (LSB) stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetHorizontalGamePadLeftStickAxis(uint player = 0);

        /// @brief Returns a Vector2 where X is the horizontal axis of the left right (RSB) and Y the vertical.
        /// See <see cref="GetHorizontalGamepadRightStickAxis"/> and/or <see cref="GetVerticalGamepadRightStickAxis"/>
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetVerticalGamePadLeftStickAxis(uint player = 0);

        /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the right (RSB) stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetHorizontalGamePadRightStickAxis(uint player = 0);

        /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of right (RSB) the stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetVerticalGamePadRightStickAxis(uint player = 0);

        /// @brief Returns a value between -1 and 1 indicating how hard the left trigger (LT button) is pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetGamePadLeftTriggerAxis(uint player = 0);

        /// @brief Returns a value between -1 and 1 indicating how hard the right trigger (RT button) is pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetGamePadRightTriggerAxis(uint player = 0);

        /// @brief Checks whether a given gamepad is connected and detected.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadConnected(uint player = 0);

        /* --- CALLBACKS --- */
        static void KeyboardCharacterCallback(GLFWwindow *windowPtr, uint character);
        static void KeyboardKeyCallback(GLFWwindow* windowPtr, int keyCode, int scanCode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* windowPtr, int buttonCode, int action, int mods);
        static void MouseScrollCallback(GLFWwindow* windowPtr, double xScroll, double yScroll);
        static void JoystickCallback(int joystickID, int event);

    private:
        static inline uint keyboardKeys[349];
        static inline uint lastKeySet = 0;
        static inline bool keySet = false;

        static inline std::vector<String> enteredCharacters;

        static inline uint mouseButtons[349];
        static inline uint lastButtonSet = 0;
        static inline bool buttonSet = false;

        static inline Vector2 scroll = { 0.0f, 0.0f };
        static inline bool scrollSet = false;

        static inline uint gamePadsConnected = 0;
        static String UnicodePointToChar(uint unicodePoint);

        struct GamePad
        {
            bool connected;
            String name;
            uint buttons[15];
            float minimumSensitivities[2];
            Vector2 axes[2];
            float triggers[2];
        };

        static inline GamePad gamePads[MAX_GAME_PADS];
        static void RegisterGamePad(uint player);
        static bool CheckGamePadConnection(uint player);
    };

}