//
// Created by Nikolay Kanchevski on 29.09.22.
//

#pragma once

enum class Key
{
    UNKNOWN = GLFW_KEY_UNKNOWN,
    SPACE = GLFW_KEY_SPACE,
    APOSTROPHE = GLFW_KEY_APOSTROPHE,
    COMMA = GLFW_KEY_COMMA,
    MINUS = GLFW_KEY_MINUS,
    PERIOD = GLFW_KEY_PERIOD,
    SLASH = GLFW_KEY_SLASH,
    NUM0 = GLFW_KEY_0,
    NUM1 = GLFW_KEY_1,
    NUM2 = GLFW_KEY_2,
    NUM3 = GLFW_KEY_3,
    NUM4 = GLFW_KEY_4,
    NUM5 = GLFW_KEY_5,
    NUM6 = GLFW_KEY_6,
    NUM7 = GLFW_KEY_7,
    NUM8 = GLFW_KEY_8,
    NUM9 = GLFW_KEY_9,
    SEMICOLON = GLFW_KEY_SEMICOLON,
    EQUAL = GLFW_KEY_EQUAL,
    A = GLFW_KEY_A,
    B = GLFW_KEY_B,
    C = GLFW_KEY_C,
    D = GLFW_KEY_D,
    E = GLFW_KEY_E,
    F = GLFW_KEY_F,
    G = GLFW_KEY_G,
    H = GLFW_KEY_H,
    I = GLFW_KEY_I,
    J = GLFW_KEY_J,
    K = GLFW_KEY_K,
    L = GLFW_KEY_L,
    M = GLFW_KEY_M,
    N = GLFW_KEY_N,
    O = GLFW_KEY_O,
    P = GLFW_KEY_P,
    Q = GLFW_KEY_Q,
    R = GLFW_KEY_R,
    S = GLFW_KEY_S,
    T = GLFW_KEY_T,
    U = GLFW_KEY_U,
    V = GLFW_KEY_V,
    W = GLFW_KEY_W,
    X = GLFW_KEY_X,
    Y = GLFW_KEY_Y,
    Z = GLFW_KEY_Z,
    LEFT_BRACKET = GLFW_KEY_LEFT_BRACKET,
    BACKSLASH = GLFW_KEY_BACKSLASH,
    RIGHT_BRACKET = GLFW_KEY_RIGHT_BRACKET,
    GRAVE_ACCENT = GLFW_KEY_GRAVE_ACCENT,
    WORLD_1 = GLFW_KEY_WORLD_1,
    WORLD_2 = GLFW_KEY_WORLD_2,
    ESCAPE = GLFW_KEY_ESCAPE,
    ENTER = GLFW_KEY_ENTER,
    TAB = GLFW_KEY_TAB,
    BACKSPACE = GLFW_KEY_BACKSPACE,
    INSERT = GLFW_KEY_INSERT,
    DELETE = GLFW_KEY_DELETE,
    RIGHT = GLFW_KEY_RIGHT,
    LEFT = GLFW_KEY_LEFT,
    DOWN = GLFW_KEY_DOWN,
    UP = GLFW_KEY_UP,
    PAGE_UP = GLFW_KEY_PAGE_UP,
    PAGE_DOWN = GLFW_KEY_PAGE_DOWN,
    HOME = GLFW_KEY_HOME,
    END = GLFW_KEY_END,
    CAPS_LOCK = GLFW_KEY_CAPS_LOCK,
    SCROLL_LOCK = GLFW_KEY_SCROLL_LOCK,
    NUM_LOCK = GLFW_KEY_NUM_LOCK,
    PRINT_SCREEN = GLFW_KEY_PRINT_SCREEN,
    PAUSE = GLFW_KEY_PAUSE,
    F1 = GLFW_KEY_F1,
    F2 = GLFW_KEY_F2,
    F3 = GLFW_KEY_F3,
    F4 = GLFW_KEY_F4,
    F5 = GLFW_KEY_F5,
    F6 = GLFW_KEY_F6,
    F7 = GLFW_KEY_F7,
    F8 = GLFW_KEY_F8,
    F9 = GLFW_KEY_F9,
    F10 = GLFW_KEY_F10,
    F11 = GLFW_KEY_F11,
    F12 = GLFW_KEY_F12,
    F13 = GLFW_KEY_F13,
    F14 = GLFW_KEY_F14,
    F15 = GLFW_KEY_F15,
    F16 = GLFW_KEY_F16,
    F17 = GLFW_KEY_F17,
    F18 = GLFW_KEY_F18,
    F19 = GLFW_KEY_F19,
    F20 = GLFW_KEY_F20,
    F21 = GLFW_KEY_F21,
    F22 = GLFW_KEY_F22,
    F23 = GLFW_KEY_F23,
    F24 = GLFW_KEY_F24,
    F25 = GLFW_KEY_F25,
    KP_0 = GLFW_KEY_KP_0,
    KP_1 = GLFW_KEY_KP_1,
    KP_2 = GLFW_KEY_KP_2,
    KP_3 = GLFW_KEY_KP_3,
    KP_4 = GLFW_KEY_KP_4,
    KP_5 = GLFW_KEY_KP_5,
    KP_6 = GLFW_KEY_KP_6,
    KP_7 = GLFW_KEY_KP_7,
    KP_8 = GLFW_KEY_KP_8,
    KP_9 = GLFW_KEY_KP_9,
    KP_DECIMAL = GLFW_KEY_KP_DECIMAL,
    KP_DIVIDE = GLFW_KEY_KP_DIVIDE,
    KP_MULTIPLY = GLFW_KEY_KP_MULTIPLY,
    KP_SUBTRACT = GLFW_KEY_KP_SUBTRACT,
    KP_ADD = GLFW_KEY_KP_ADD,
    KP_ENTER = GLFW_KEY_KP_ENTER,
    KP_EQUAL = GLFW_KEY_KP_EQUAL,
    LEFT_SHIFT = GLFW_KEY_LEFT_SHIFT,
    LEFT_CONTROL = GLFW_KEY_LEFT_CONTROL,
    LEFT_ALT = GLFW_KEY_LEFT_ALT,
    LEFT_SUPER = GLFW_KEY_LEFT_SUPER,
    RIGHT_SHIFT = GLFW_KEY_RIGHT_SHIFT,
    RIGHT_CONTROL = GLFW_KEY_RIGHT_CONTROL,
    RIGHT_ALT = GLFW_KEY_RIGHT_ALT,
    RIGHT_SUPER = GLFW_KEY_RIGHT_SUPER,
    MENU = GLFW_KEY_MENU
};

enum class MouseButton
{
    NUM1 = GLFW_MOUSE_BUTTON_1,
    NUM2 = GLFW_MOUSE_BUTTON_2,
    NUM3 = GLFW_MOUSE_BUTTON_3,
    NUM4 = GLFW_MOUSE_BUTTON_4,
    NUM5 = GLFW_MOUSE_BUTTON_5,
    NUM6 = GLFW_MOUSE_BUTTON_6,
    NUM7 = GLFW_MOUSE_BUTTON_7,
    NUM8 = GLFW_MOUSE_BUTTON_8,
    LEFT = GLFW_MOUSE_BUTTON_1,
    RIGHT = GLFW_MOUSE_BUTTON_2,
    MIDDLE = GLFW_MOUSE_BUTTON_3
};

enum class GamePadButton
{
    A = GLFW_GAMEPAD_BUTTON_A,
    B = GLFW_GAMEPAD_BUTTON_B,
    X = GLFW_GAMEPAD_BUTTON_X,
    Y = GLFW_GAMEPAD_BUTTON_Y,
    CROSS = GLFW_GAMEPAD_BUTTON_CROSS,
    CIRCLE = GLFW_GAMEPAD_BUTTON_CIRCLE,
    SQUARE = GLFW_GAMEPAD_BUTTON_SQUARE,
    TRIANGLE = GLFW_GAMEPAD_BUTTON_TRIANGLE,
    LEFT_BUMPER = GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,
    RIGHT_BUMPER = GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
    BACK = GLFW_GAMEPAD_BUTTON_BACK,
    START = GLFW_GAMEPAD_BUTTON_START,
    GUIDE = GLFW_GAMEPAD_BUTTON_GUIDE,
    LEFT_THUMB = GLFW_GAMEPAD_BUTTON_LEFT_THUMB,
    RIGHT_THUMB = GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
    DPAD_UP = GLFW_GAMEPAD_BUTTON_DPAD_UP,
    DPAD_RIGHT = GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,
    DPAD_DOWN = GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
    DPAD_LEFT = GLFW_GAMEPAD_BUTTON_DPAD_LEFT
};

/// @brief Contains useful methods for keyboard, mouse, and gamepad input querying.
namespace Sierra::Engine::Input
{

    /* --- PROPERTIES --- */
    constexpr uint32 MAX_GAME_PADS = 8;

    /* --- POLLING METHODS --- */
    void Initialize();
    void Update();

    /* --- SETTER METHODS --- */
    void SetGamePadMinimumStickSensitivity(float minimumSensitivity, uint32 player = 0);
    void SetGamePadMinimumLeftStickSensitivity(float minimumSensitivity, uint32 player = 0);
    void SetGamePadMinimumRightStickSensitivity(float minimumSensitivity, uint32 player = 0);

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
    [[nodiscard]] String GetGamePadName(uint32 player = 0);

    /// @brief Checks whether a button on a given gamepad is pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonPressed(GamePadButton button, uint32 player = 0);

    /// @brief Checks whether a button on a given gamepad is held. Also returns true if pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonHeld(GamePadButton button, uint32 player = 0);

    /// @brief Checks whether a button on a given gamepad is released.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonReleased(GamePadButton button, uint32 player = 0);

    /// @brief Checks whether a button on a given gamepad is resting - is not pressed, held, or released.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param gamepadButton The button to check.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadButtonResting(GamePadButton button, uint32 player = 0);

    /// @brief Returns a Vector2 where X is the horizontal axis of the left stick (LSB) and Y the vertical.
    /// See <see cref="GetHorizontalGamepadLeftStickAxis"/> and/or <see cref="GetVerticalGamepadLeftStickAxis"/>
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] Vector2 GetGamePadLeftStickAxis(uint32 player = 0);

    /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the left (LSB) stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] Vector2 GetGamePadRightStickAxis(uint32 player = 0);


    /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of the left (LSB) stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetHorizontalGamePadLeftStickAxis(uint32 player = 0);

    /// @brief Returns a Vector2 where X is the horizontal axis of the left right (RSB) and Y the vertical.
    /// See <see cref="GetHorizontalGamepadRightStickAxis"/> and/or <see cref="GetVerticalGamepadRightStickAxis"/>
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetVerticalGamePadLeftStickAxis(uint32 player = 0);

    /// @brief Returns a value between -1 and 1 indicating the current input on the horizontal axis of the right (RSB) stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetHorizontalGamePadRightStickAxis(uint32 player = 0);

    /// @brief Returns a value between -1 and 1 indicating the current input on the vertical axis of right (RSB) the stick.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetVerticalGamePadRightStickAxis(uint32 player = 0);

    /// @brief Returns a value between -1 and 1 indicating how hard the left trigger (LT button) is pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetGamePadLeftTriggerAxis(uint32 player = 0);

    /// @brief Returns a value between -1 and 1 indicating how hard the right trigger (RT button) is pressed.
    /// See also <a href="https://gist.github.com/palmerj/586375bcc5bc83ccdaf00c6f5f863e86 this link</a>
    /// if you are not familiar with the buttons and layout of gamepads.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] float GetGamePadRightTriggerAxis(uint32 player = 0);

    /// @brief Checks whether a given gamepad is connected and detected.
    /// @param player Which player's gamepad to use (also referred to as an ID of the gamepad).
    [[nodiscard]] bool GetGamePadConnected(uint32 player = 0);

    /* --- CALLBACKS --- */
    void KeyboardCharacterCallback(GLFWwindow *windowPtr, uint32 character);
    void KeyboardKeyCallback(GLFWwindow* windowPtr, int32 keyCode, int32 scanCode, int32 action, int32 mods);
    void MouseButtonCallback(GLFWwindow* windowPtr, int32 buttonCode, int32 action, int32 mods);
    void MouseScrollCallback(GLFWwindow* windowPtr, double xScroll, double yScroll);
    void JoystickCallback(int32 joystickID, int32 event);

}