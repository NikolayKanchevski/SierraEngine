//
// Created by Nikolay Kanchevski on 29.09.22.
//

#include <glm/common.hpp>
#include <iostream>
#include <codecvt>
#include "Input.h"
#include "../../Core/Rendering/Vulkan/VulkanDebugger.h"

using namespace Sierra::Core::Rendering::Vulkan;

namespace Sierra::Engine::Classes {

    /* --- STATIC FIELDS IMPLEMENTATION --- */
    uint32_t Input::keyboardKeys[349];
    uint32_t Input::lastKeySet;
    bool Input::keySet;
    std::vector<std::string> Input::enteredCharacters;

    uint32_t Input::mouseButtons[349];
    uint32_t Input::lastButtonSet;
    bool Input::buttonSet;

    glm::vec2 Input::scroll;
    bool Input::scrollSet;

    uint32_t Input::MAX_GAME_PADS = 8;
    uint32_t Input::gamePadsConnected = 0;
    Input::GamePad Input::gamePads[8];

    /* --- POLLING METHODS --- */

    void Input::Start()
    {
        for (int i = (int) MAX_GAME_PADS + 1; i--;)
        {
            if (glfwJoystickPresent(i))
            {
                if (glfwJoystickIsGamepad(i))
                {
                    RegisterGamePad(i);
                }
            }
        }
    }

    void Input::Update()
    {
        if (scrollSet)
        {
            scroll = { 0, 0 };

            scrollSet = false;
        }

        if (keySet)
        {
            if (keyboardKeys[lastKeySet] == 2) keyboardKeys[lastKeySet] = 3;
            else if (keyboardKeys[lastKeySet] == 1) keyboardKeys[lastKeySet] = 0;

            keySet = false;
        }

        if (buttonSet)
        {
            if (mouseButtons[lastButtonSet] == 2) mouseButtons[lastButtonSet] = 3;
            else if (mouseButtons[lastButtonSet] == 1) mouseButtons[lastButtonSet] = 0;

            buttonSet = false;
        }

        for (int i = 0; i < gamePadsConnected; i++)
        {
            GLFWgamepadstate gamePadState;
            glfwGetGamepadState(i, &gamePadState);

            glm::vec2 leftAxis = { gamePadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], gamePadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X] };
            gamePads[i].axes[0] = { glm::abs(leftAxis.x) >= glm::abs(gamePads[i].minimumSensitivities[0]) ? leftAxis.x : 0.0f, glm::abs(leftAxis.y) >= glm::abs(gamePads[i].minimumSensitivities[0]) ? -leftAxis.y : 0.0f };

            glm::vec2 rightAxis = { gamePadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], gamePadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] };
            gamePads[i].axes[1] = { glm::abs(rightAxis.x) >= glm::abs(gamePads[i].minimumSensitivities[1]) ? rightAxis.x : 0.0f, glm::abs(rightAxis.y) >= glm::abs(gamePads[i].minimumSensitivities[1]) ? -rightAxis.y : 0.0f };

            gamePads[i].triggers[0] = (gamePadState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1) / 2;
            gamePads[i].triggers[1] = (gamePadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1) / 2;

            for (int j = 15; j--;)
            {
                uint32_t oldState = gamePads[i].buttons[j];
                uint32_t newState = gamePadState.buttons[j] + 1;

                if (oldState == 3 && newState == 2 || oldState == 2 && newState == 2) newState = 3;
                else if (oldState == 0 && newState == 1 || oldState == 1 && newState == 1) newState = 0;

                gamePads[i].buttons[j] = newState;
            }
        }
    }

    /* --- SETTER METHODS --- */

    void Input::SetGamePadMinimumStickSensitivity(const float minimumSensitivity, const uint32_t player)
    {
        if (CheckGamePadConnection(player))
        {
            gamePads[player].minimumSensitivities[0] = minimumSensitivity;
            gamePads[player].minimumSensitivities[1] = minimumSensitivity;
        }
    }

    void Input::SetGamePadMinimumLeftStickSensitivity(const float minimumSensitivity, const uint32_t player)
    {
        if (CheckGamePadConnection(player)) gamePads[player].minimumSensitivities[0] = minimumSensitivity;
    }

    void Input::SetGamePadMinimumRightStickSensitivity(const float minimumSensitivity, const uint32_t player)
    {
        if (CheckGamePadConnection(player)) gamePads[player].minimumSensitivities[1] = minimumSensitivity;
    }

    /* --- GETTER METHODS --- */

    bool Input::GetKeyPressed(const uint32_t keyCode)
    {
        return keyboardKeys[keyCode] == 2; // 2 = Press
    }

    bool Input::GetKeyHeld(const uint32_t keyCode)
    {
        return keyboardKeys[keyCode] == 3 || keyboardKeys[keyCode] == 2; // 3 = Repeat || 2 = Press
    }

    bool Input::GetKeyResting(const uint32_t keyCode)
    {
        return keyboardKeys[keyCode] == 0; // 0 = Rest
    }

    bool Input::GetKeyReleased(const uint32_t keyCode)
    {
        return keyboardKeys[keyCode] == 1; // 1 = Release
    }

    std::vector<std::string> *Input::GetEnteredCharacters()
    {
        return &enteredCharacters;
    }

    bool Input::GetMouseButtonPressed(const uint32_t buttonCode)
    {
        return mouseButtons[buttonCode] == 2; // 2 = Press
    }

    bool Input::GetMouseButtonHeld(const uint32_t buttonCode)
    {
        return mouseButtons[buttonCode] == 3 || mouseButtons[buttonCode] == 2; // 3 = Repeat || 2 = Press
    }

    bool Input::GetMouseButtonReleased(const uint32_t buttonCode)
    {
        return mouseButtons[buttonCode] == 1; // 1 = Release
    }

    bool Input::GetMouseButtonResting(const uint32_t buttonCode)
    {
        return mouseButtons[buttonCode] == 0; // 0 = Rest
    }

    float Input::GetHorizontalMouseScroll()
    {
        return scroll.x;
    }

    float Input::GetVerticalMouseScroll()
    {
        return scroll.y;
    }

    bool Input::GetGamePadButtonPressed(const uint32_t gamePadButton, const uint32_t player)
    {
        return CheckGamePadConnection(player) && gamePads[player].buttons[gamePadButton] == 2; // 2 - Press
    }

    bool Input::GetGamePadButtonHeld(const uint32_t gamePadButton, const uint32_t player)
    {
        return CheckGamePadConnection(player) && (gamePads[player].buttons[gamePadButton] == 2 || gamePads[player].buttons[gamePadButton] == 3); // 2 - Press || 3 - Hold
    }

    bool Input::GetGamePadButtonReleased(const uint32_t gamePadButton, const uint32_t player)
    {
        return CheckGamePadConnection(player) && gamePads[player].buttons[gamePadButton] == 1; // 1 - Press
    }

    bool Input::GetGamePadButtonResting(const uint32_t gamePadButton, const uint32_t player)
    {
        return CheckGamePadConnection(player) && gamePads[player].buttons[gamePadButton] == 0; // 0 - Rest
    }

    glm::vec2 Input::GetGamePadLeftStickAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? -gamePads[player].axes[0] : glm::vec2(0, 0);
    }

    glm::vec2 Input::GetGamePadRightStickAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? -gamePads[player].axes[1] : glm::vec2(0, 0);
    }

    float Input::GetHorizontalGamePadLeftStickAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[0].x * -1 : 0.0f;
    }

    float Input::GetVerticalGamePadLeftStickAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[0].y * -1 : 0.0f;
    }

    float Input::GetHorizontalGamePadRightStickAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[1].x * -1 : 0.0f;
    }

    float Input::GetVerticalGamePadRightStickAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[1].y * -1 : 0.0f;
    }

    float Input::GetGamePadLeftTriggerAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].triggers[0] : 0.0f;
    }

    float Input::GetGamePadRightTriggerAxis(const uint32_t player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].triggers[1] : 0.0f;
    }

    bool Input::GetGamePadConnected(const uint32_t player)
    {
        return player < MAX_GAME_PADS && gamePads[player].connected;
    }

    bool Input::CheckGamePadConnection(uint32_t player)
    {
        if (gamePadsConnected < 1)
        {
            VulkanDebugger::ThrowWarning("No game pads are found on the system");
            return false;
        }

        if (player >= MAX_GAME_PADS || !gamePads[player].connected)
        {
            VulkanDebugger::ThrowWarning("Game pad with an ID of [" + std::to_string(player) + "] is not connected");
            return false;
        }

        return true;
    }

    /* --- CALLBACKS --- */

    void Input::KeyboardCharacterCallback(GLFWwindow *windowPtr, const uint32_t character)
    {
        enteredCharacters.push_back(UnicodePointToChar(character));
    }

    void Input::KeyboardKeyCallback(GLFWwindow *windowPtr, const int keyCode, const int scanCode, int action, const int mods)
    {
        if (keyCode == GLFW_KEY_UNKNOWN) { return; }

        keyboardKeys[keyCode] = action + 1;
        lastKeySet = keyCode;
        keySet = true;
    }

    void Input::MouseButtonCallback(GLFWwindow *windowPtr, const int buttonCode, const int action, const int mods)
    {
        mouseButtons[buttonCode] = action + 1;
        lastButtonSet = buttonCode;
        buttonSet = true;
    }

    void Input::MouseScrollCallback(GLFWwindow *windowPtr, double xScroll, double yScroll)
    {
        if (glm::abs(xScroll) >= glm::abs(yScroll)) yScroll = 0;
        else if (glm::abs(xScroll) < glm::abs(yScroll)) xScroll = 0;

        scroll = { (float) xScroll, (float) yScroll };
        scrollSet = true;
    }

    void Input::JoystickCallback(const int joystickID, const int connectionStatus)
    {
        if (connectionStatus == GLFW_CONNECTED)
        {
            if (glfwJoystickIsGamepad(joystickID))
            {
                RegisterGamePad(joystickID);
                gamePadsConnected++;
            }
        }
        else
        {
            if (gamePadsConnected < 1) return;

            gamePads[joystickID].connected = false;
            gamePadsConnected--;
        }
    }

    void Input::RegisterGamePad(const uint32_t player)
    {
        gamePads[player].connected = true;
        gamePads[player].name = glfwGetGamepadName((int) player);
    }

    /* --- PRIVATE METHODS --- */

#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
    std::string Input::UnicodePointToChar(const uint32_t unicodePoint)
    {
        char character[5] = {0x00, 0x00, 0x00, 0x00, 0x00 };
        if      (unicodePoint <= 0x7F) { character[0] = unicodePoint;  }
        else if (unicodePoint <= 0x7FF) { character[0] = (unicodePoint >> 6) + 192; character[1] = (unicodePoint & 63) + 128; }
        else if (0xd800 <= unicodePoint && unicodePoint <= 0xdfff) { } // Invalid block of utf8
        else if (unicodePoint <= 0xFFFF) { character[0] = (unicodePoint >> 12) + 224; character[1] = ((unicodePoint >> 6) & 63) + 128; character[2] = (unicodePoint & 63) + 128; }
        else if (unicodePoint <= 0x10FFFF) { character[0] = (unicodePoint >> 18) + 240; character[1] = ((unicodePoint >> 12) & 63) + 128; character[2] = (( unicodePoint >> 6) & 63) + 128; character[3] = (unicodePoint & 63) + 128; }
        return {character };
    }
#pragma clang diagnostic pop
}
