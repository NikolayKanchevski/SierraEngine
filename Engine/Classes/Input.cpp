//
// Created by Nikolay Kanchevski on 29.09.22.
//

#include "Input.h"

namespace Sierra::Engine
{
    
    /* --- PROPERTIES --- */
    
    uint keyboardKeys[349];
    uint lastKeySet = 0;
    bool keySet = false;

    uint mouseButtons[349];
    uint lastButtonSet = 0;
    bool buttonSet = false;

    Vector2 scroll = { 0.0f, 0.0f };
    bool scrollSet = false;

    std::vector<String> enteredCharacters;
    String UnicodePointToChar(uint unicodePoint);

    struct GamePad
    {
        bool connected;
        String name;
        uint buttons[15];
        float minimumSensitivities[2];
        Vector2 axes[2];
        float triggers[2];
    };

    GamePad gamePads[Input::MAX_GAME_PADS];
    uint gamePadsConnected = 0;

    void RegisterGamePad(uint player);
    bool CheckGamePadConnection(uint player);

    /* --- POLLING METHODS --- */

    void Input::Start()
    {
        for (uint i = 0; i < MAX_GAME_PADS; i++)
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
        if (scrollSet) scrollSet = false;
        else
        {
            scroll = { 0, 0 };

            scrollSet = false;
        }

        if (keySet) keySet = false;
        else
        {
            if (keyboardKeys[lastKeySet] == 2) keyboardKeys[lastKeySet] = 3;
            else if (keyboardKeys[lastKeySet] == 1) keyboardKeys[lastKeySet] = 0;

            keySet = false;
        }

        if (buttonSet) buttonSet = false;
        else
        {
            if (mouseButtons[lastButtonSet] == 2) mouseButtons[lastButtonSet] = 3;
            else if (mouseButtons[lastButtonSet] == 1) mouseButtons[lastButtonSet] = 0;

            buttonSet = false;
        }

        for (uint i = MAX_GAME_PADS; i--;)
        {
            if (!gamePads[i].connected) continue;

            GLFWgamepadstate gamePadState;
            glfwGetGamepadState(i, &gamePadState);

            Vector2 leftAxis = { gamePadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], gamePadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] };
            gamePads[i].axes[0] = { glm::abs(leftAxis.x) >= glm::abs(gamePads[i].minimumSensitivities[0]) ? leftAxis.x : 0.0f, glm::abs(leftAxis.y) >= glm::abs(gamePads[i].minimumSensitivities[0]) ? -leftAxis.y : 0.0f };

            Vector2 rightAxis = { gamePadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], gamePadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] };
            gamePads[i].axes[1] = { glm::abs(rightAxis.x) >= glm::abs(gamePads[i].minimumSensitivities[1]) ? rightAxis.x : 0.0f, glm::abs(rightAxis.y) >= glm::abs(gamePads[i].minimumSensitivities[1]) ? -rightAxis.y : 0.0f };

            gamePads[i].triggers[0] = (gamePadState.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] + 1) / 2;
            gamePads[i].triggers[1] = (gamePadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] + 1) / 2;

            for (int j = 15; j--;)
            {
                uint oldState = gamePads[i].buttons[j];
                uint newState = gamePadState.buttons[j] + 1;

                if (oldState == 3 && newState == 2 || oldState == 2 && newState == 2) newState = 3;
                else if (oldState == 0 && newState == 1 || oldState == 1 && newState == 1) newState = 0;

                gamePads[i].buttons[j] = newState;
            }
        }
    }

    /* --- SETTER METHODS --- */

    void Input::SetGamePadMinimumStickSensitivity(const float minimumSensitivity, const uint player)
    {
        if (CheckGamePadConnection(player))
        {
            gamePads[player].minimumSensitivities[0] = minimumSensitivity;
            gamePads[player].minimumSensitivities[1] = minimumSensitivity;
        }
    }

    void Input::SetGamePadMinimumLeftStickSensitivity(const float minimumSensitivity, const uint player)
    {
        if (CheckGamePadConnection(player)) gamePads[player].minimumSensitivities[0] = minimumSensitivity;
    }

    void Input::SetGamePadMinimumRightStickSensitivity(const float minimumSensitivity, const uint player)
    {
        if (CheckGamePadConnection(player)) gamePads[player].minimumSensitivities[1] = minimumSensitivity;
    }

    /* --- GETTER METHODS --- */

    bool Input::GetKeyPressed(const Key key)
    {
        return keyboardKeys[static_cast<uint>(key)] == 2; // 2 = Press
    }

    bool Input::GetKeyHeld(const Key key)
    {
        return keyboardKeys[static_cast<uint>(key)] == 3 || keyboardKeys[static_cast<uint>(key)] == 2; // 3 = Repeat || 2 = Press
    }

    bool Input::GetKeyResting(const Key key)
    {
        return keyboardKeys[static_cast<uint>(key)] == 0; // 0 = Rest
    }

    bool Input::GetKeyReleased(const Key key)
    {
        return keyboardKeys[static_cast<uint>(key)] == 1; // 1 = Release
    }

    std::vector<String> *Input::GetEnteredCharacters()
    {
        return &enteredCharacters;
    }

    bool Input::GetMouseButtonPressed(const MouseButton button)
    {
        return mouseButtons[static_cast<uint>(button)] == 2; // 2 = Press
    }

    bool Input::GetMouseButtonHeld(const MouseButton button)
    {
        return mouseButtons[static_cast<uint>(button)] == 3 || mouseButtons[static_cast<uint>(button)] == 2; // 3 = Repeat || 2 = Press
    }

    bool Input::GetMouseButtonReleased(const MouseButton button)
    {
        return mouseButtons[static_cast<uint>(button)] == 1; // 1 = Release
    }

    bool Input::GetMouseButtonResting(const MouseButton button)
    {
        return mouseButtons[static_cast<uint>(button)] == 0; // 0 = Rest
    }

    float Input::GetHorizontalMouseScroll()
    {
        return scroll.x;
    }

    float Input::GetVerticalMouseScroll()
    {
        return scroll.y;
    }

    String Input::GetGamePadName(uint player)
    {
        return gamePads[player].name;
    }

    bool Input::GetGamePadButtonPressed(const GamePadButton button, const uint player)
    {
        return CheckGamePadConnection(player) && gamePads[player].buttons[static_cast<uint>(button)] == 2; // 2 - Press
    }

    bool Input::GetGamePadButtonHeld(const GamePadButton button, const uint player)
    {
        return CheckGamePadConnection(player) && (gamePads[player].buttons[static_cast<uint>(button)] == 2 || gamePads[player].buttons[static_cast<uint>(button)] == 3); // 2 - Press || 3 - Hold
    }

    bool Input::GetGamePadButtonReleased(const GamePadButton button, const uint player)
    {
        return CheckGamePadConnection(player) && gamePads[player].buttons[static_cast<uint>(button)] == 1; // 1 - Press
    }

    bool Input::GetGamePadButtonResting(const GamePadButton button, const uint player)
    {
        return CheckGamePadConnection(player) && gamePads[player].buttons[static_cast<uint>(button)] == 0; // 0 - Rest
    }

    Vector2 Input::GetGamePadLeftStickAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[0] : Vector2(0, 0);
    }

    Vector2 Input::GetGamePadRightStickAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[1] : Vector2(0, 0);
    }

    float Input::GetHorizontalGamePadLeftStickAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[0].x : 0.0f;
    }

    float Input::GetVerticalGamePadLeftStickAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[0].y : 0.0f;
    }

    float Input::GetHorizontalGamePadRightStickAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[1].x : 0.0f;
    }

    float Input::GetVerticalGamePadRightStickAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].axes[1].y : 0.0f;
    }

    float Input::GetGamePadLeftTriggerAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].triggers[0] : 0.0f;
    }

    float Input::GetGamePadRightTriggerAxis(const uint player)
    {
        return CheckGamePadConnection(player) ? gamePads[player].triggers[1] : 0.0f;
    }

    bool Input::GetGamePadConnected(const uint player)
    {
        return player < MAX_GAME_PADS && gamePads[player].connected;
    }

    bool CheckGamePadConnection(uint player)
    {
        if (player >= Input::MAX_GAME_PADS || !gamePads[player].connected)
        {
            ASSERT_WARNING_FORMATTED("Game pad with an ID of [{0}] is not connected", player);
            return false;
        }

        return true;
    }

    /* --- CALLBACKS --- */

    void Input::KeyboardCharacterCallback([[maybe_unused]] GLFWwindow *windowPtr, const uint character)
    {
        enteredCharacters.push_back(UnicodePointToChar(character));
    }

    void Input::KeyboardKeyCallback([[maybe_unused]] GLFWwindow *windowPtr, const int keyCode, const int scanCode, int action, const int mods)
    {
        if (keyCode == static_cast<uint>(Key::UNKNOWN)) return;

        keyboardKeys[static_cast<uint>(keyCode)] = action + 1;
        lastKeySet = keyCode;
        keySet = true;
    }

    void Input::MouseButtonCallback([[maybe_unused]] GLFWwindow *windowPtr, const int button, const int action, const int mods)
    {
        mouseButtons[static_cast<uint>(button)] = action + 1;
        lastButtonSet = button;
        buttonSet = true;
    }

    void Input::MouseScrollCallback([[maybe_unused]] GLFWwindow *windowPtr, double xScroll, double yScroll)
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
            gamePads[joystickID].connected = false;
            gamePadsConnected--;
        }
    }

    void RegisterGamePad(const uint player)
    {
        gamePads[player].connected = true;
        gamePads[player].name = glfwGetGamepadName(static_cast<int>(player));
        gamePads[player].minimumSensitivities[0] = 0.2f;
        gamePads[player].minimumSensitivities[1] = 0.2f;
    }

    /* --- PRIVATE METHODS --- */

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "cppcoreguidelines-narrowing-conversions"
    String UnicodePointToChar(const uint unicodePoint)
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
