//
// Created by Nikolay Kanchevski on 29.09.22.
//

#pragma once

#include <vector>
#include <glm/vec2.hpp>
#include <GLFW/glfw3.h>
#include <string>

namespace Sierra::Engine::Classes
{

    /// @brief Contains useful methods for keyboard, mouse, and gamepad input querying.
    class Input
    {
    public:
        /* --- PROPERTIES --- */
        static uint32_t MAX_GAME_PADS;

        /* --- POLLING METHODS --- */
        static void Start();
        static void Update();

        /* --- SETTER METHODS --- */
        static void SetGamePadMinimumStickSensitivity(float minimumSensitivity, uint32_t player = 0);
        static void SetGamePadMinimumLeftStickSensitivity(float minimumSensitivity, uint32_t player = 0);
        static void SetGamePadMinimumRightStickSensitivity(float minimumSensitivity, uint32_t player = 0);

        /* --- GETTER METHODS --- */
        /// @brief Checks whether a key on the keyboard is pressed.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyPressed(uint32_t keyCode);

        /// @brief Checks whether a key on the keyboard is held. Also returns true if pressed.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyHeld(uint32_t keyCode);

        /// @brief Checks whether a key on the keyboard is released.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyReleased(uint32_t keyCode);

        /// @brief Checks whether a key on the keyboard is resting - is not pressed, held, or released.
        /// @param keyCode The key to check.
        [[nodiscard]] static bool GetKeyResting(uint32_t keyCode);

        /// @brief Returns the currently inputted from the keyboard characters.
        [[nodiscard]] static std::vector<std::string> *GetEnteredCharacters();

        /// @brief Checks whether a button on the mouse is pressed.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonPressed(uint32_t buttonCode);

        /// @brief Checks whether a button on the mouse is held. Also returns true if pressed.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonHeld(uint32_t buttonCode);

        /// @brief Checks whether a button on the mouse is released.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonReleased(uint32_t buttonCode);

        /// @brief Checks whether a button on the mouse is resting - is not pressed, held, or released.
        /// @param buttonCode The button to check.
        [[nodiscard]] static bool GetMouseButtonResting(uint32_t buttonCode);

        /// @brief Returns the horizontal mouse scroll delta. Horizontal scrolling is usually only supported on laptops,
        /// or special external input devices.
        [[nodiscard]] static float GetHorizontalMouseScroll();

        /// @brief Returns the vertical mouse scroll delta.
        [[nodiscard]] static float GetVerticalMouseScroll();

        /// @brief Gets the model tag of a given player's game pad.
        [[nodiscard]] static std::string GetGamePadName(uint32_t player = 0);

        /// @brief Checks whether a button on a given gamepad is pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonPressed(uint32_t gamePadButton, uint32_t player = 0);

        /// @brief Checks whether a button on a given gamepad is held. Also returns true if pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonHeld(uint32_t gamePadButton, uint32_t player = 0);

        /// @brief Checks whether a button on a given gamepad is released.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonReleased(uint32_t gamePadButton, uint32_t player = 0);

        /// @brief Checks whether a button on a given gamepad is resting - is not pressed, held, or released.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param gamepadButton The button to check.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadButtonResting(uint32_t gamePadButton, uint32_t player = 0);

        /// @brief Returns the a Vector2 where X is the horizontal axis of the left stick (LSB) and Y the vertical.
        /// See <see cref="GetHorizontalGamepadLeftStickAxis"/> and/or <see cref="GetVerticalGamepadLeftStickAxis"/>
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static glm::vec2 GetGamePadLeftStickAxis(uint32_t player = 0);

        /// @brief Returns the a value between -1 and 1 indicating the current input on the horizontal axis of the left (LSB) stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static glm::vec2 GetGamePadRightStickAxis(uint32_t player = 0);


        /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of the left (LSB) stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetHorizontalGamePadLeftStickAxis(uint32_t player = 0);

        /// @brief Returns a Vector2 where X is the horizontal axis of the left right (RSB) and Y the vertical.
        /// See <see cref="GetHorizontalGamepadRightStickAxis"/> and/or <see cref="GetVerticalGamepadRightStickAxis"/>
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetVerticalGamePadLeftStickAxis(uint32_t player = 0);

        /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the right (RSB) stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetHorizontalGamePadRightStickAxis(uint32_t player = 0);

        /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of right (RSB) the stick.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetVerticalGamePadRightStickAxis(uint32_t player = 0);

        /// @brief Returns a value between -1 and 1 indicating how hard the left trigger (LT button) is pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetGamePadLeftTriggerAxis(uint32_t player = 0);

        /// @brief Returns a value between -1 and 1 indicating how hard the right trigger (RT button) is pressed.
        /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
        /// if you are not familiar with the buttons and layout of gamepads.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static float GetGamePadRightTriggerAxis(uint32_t player = 0);

        /// @brief Checks whether a given gamepad is connected and detected.
        /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
        [[nodiscard]] static bool GetGamePadConnected(uint32_t player = 0);

        /* --- CALLBACKS --- */
        static void KeyboardCharacterCallback(GLFWwindow *windowPtr, uint32_t character);
        static void KeyboardKeyCallback(GLFWwindow* windowPtr, int keyCode, int scanCode, int action, int mods);
        static void MouseButtonCallback(GLFWwindow* windowPtr, int buttonCode, int action, int mods);
        static void MouseScrollCallback(GLFWwindow* windowPtr, double xScroll, double yScroll);
        static void JoystickCallback(int joystickID, int event);

        /* --- DESTRUCTOR --- */
        Input() = delete;

    private:
        static uint32_t keyboardKeys[349];
        static uint32_t lastKeySet;
        static bool keySet;

        static std::vector<std::string> enteredCharacters;

        static uint32_t mouseButtons[349];
        static uint32_t lastButtonSet;
        static bool buttonSet;

        static glm::vec2 scroll;
        static bool scrollSet;

        static uint32_t gamePadsConnected;

        static std::string UnicodePointToChar(uint32_t unicodePoint);

        struct GamePad
        {
            bool connected;
            std::string name;
            uint32_t buttons[15];
            float minimumSensitivities[2] = { 0.2f, 0.2f };
            glm::vec2 axes[2];
            float triggers[2];
        };

        static GamePad gamePads[];
        static void RegisterGamePad(uint32_t player);
        static bool CheckGamePadConnection(uint32_t player);
    };

}