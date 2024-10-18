//
// Created by Nikolay Kanchevski on 12/14/23.
//

#include "GameKitWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GameKitWindow::GameKitWindow(const GameKitContext& gameKitContext, const WindowCreateInfo& createInfo)
        : Window(createInfo), gameKitContext(gameKitContext), title(createInfo.title)
    {
        window = gameKitContext.CreateWindow(OnCommand, this);
    }

    /* --- POLLING METHODS --- */

    void GameKitWindow::Update()
    {
        touchManager.Update(gameKitContext.GetApp()->inputBuffers[0]);
    }

    void GameKitWindow::Minimize()
    {
        // Not applicable
    }

    void GameKitWindow::Maximize()
    {
        // Not applicable
    }

    void GameKitWindow::Show()
    {
        // Not applicable
    }

    void GameKitWindow::Hide()
    {
        // Not applicable
    }

    void GameKitWindow::Focus()
    {
        // Not applicable
    }

    void GameKitWindow::Close()
    {
        if (closed) return;

        closed = true;
        GetWindowCloseDispatcher().DispatchEvent();

        gameKitContext.DestroyWindow(window);
        window = nullptr;
    }

    /* --- SETTER METHODS --- */

    void GameKitWindow::SetTitle(const std::string_view newTitle)
    {
        title = newTitle;
    }

    void GameKitWindow::SetPosition(const Vector2Int position)
    {
        // Not applicable
    }

    void GameKitWindow::SetSize(const Vector2UInt size)
    {
        // Not applicable
    }

    void GameKitWindow::SetOpacity(float32 opacity)
    {
        // Not applicable
    }

    /* --- GETTER METHODS --- */

    std::string_view GameKitWindow::GetTitle() const noexcept
    {
        return title;
    }

    Vector2Int GameKitWindow::GetPosition() const noexcept
    {
        return { 0, 0 };
    }
    
    uint32 GameKitWindow::GetWidth() const noexcept
    {
        return ANativeWindow_getWidth(window);
    }

    uint32 GameKitWindow::GetHeight() const noexcept
    {
        return ANativeWindow_getHeight(window);
    }

    uint32 GameKitWindow::GetFramebufferWidth() const noexcept
    {
        return ANativeWindow_getWidth(window);
    }

    uint32 GameKitWindow::GetFramebufferHeight() const noexcept
    {
        return ANativeWindow_getHeight(window);
    }

    float32 GameKitWindow::GetOpacity() const noexcept
    {
        return 1.0f;
    }

    bool GameKitWindow::IsClosed() const noexcept
    {
        return closed;
    }

    bool GameKitWindow::IsMinimized() const noexcept
    {
        return minimized;
    }

    bool GameKitWindow::IsMaximized() const noexcept
    {
        return !minimized;
    }

    bool GameKitWindow::IsFocused() const noexcept
    {
        return !minimized;
    }

    bool GameKitWindow::IsHidden() const noexcept
    {
        return minimized;
    }

    InputManager* GameKitWindow::GetInputManager() noexcept
    {
        return nullptr;
    }

    CursorManager* GameKitWindow::GetCursorManager() noexcept
    {
        return nullptr;
    }

    TouchManager* GameKitWindow::GetTouchManager() noexcept
    {
        return &touchManager;
    }

    WindowingBackendType GameKitWindow::GetBackendType() const noexcept
    {
        return WindowingBackendType::GameKit;
    }

    /* --- DESTRUCTOR --- */

    GameKitWindow::~GameKitWindow() noexcept
    {
        if (closed) return;

        closed = true;
        GetWindowCloseDispatcher().DispatchEvent();

        gameKitContext.DestroyWindow(window);
    }

    /* --- PRIVATE METHODS --- */

    void GameKitWindow::OnCommand(const uint32 command, void* const userData)
    {
        GameKitWindow* const window = reinterpret_cast<GameKitWindow*>(userData);
        if (window == nullptr) return;

        switch (command)
        {
            case APP_CMD_GAINED_FOCUS:
            {
                window->minimized = false;
                window->GetWindowFocusDispatcher().DispatchEvent(true);
                break;
            }
            case APP_CMD_LOST_FOCUS:
            {
                window->minimized = true;
                window->GetWindowFocusDispatcher().DispatchEvent(false);
                break;
            }
            case APP_CMD_WINDOW_RESIZED:
            {
                const uint32 width = static_cast<uint32>(ANativeWindow_getWidth(window->window));
                const uint32 height = static_cast<uint32>(ANativeWindow_getHeight(window->window));
                window->GetWindowResizeDispatcher().DispatchEvent(width, height);
            }
            case APP_CMD_TOUCH_EVENT:
            {
                if (const android_input_buffer* inputBuffer = window->gameKitContext.GetCurrentInputBuffer(); inputBuffer != nullptr)
                {
                    for (size i = 0; i < inputBuffer->motionEventsCount; i++)
                    {
                        const GameActivityMotionEvent& motionEvent = inputBuffer->motionEvents[i];
                        const GameActivityPointerAxes& pointer = motionEvent.pointers[(motionEvent.action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT];

                        switch (motionEvent.action)
                        {
                            case AMOTION_EVENT_ACTION_DOWN:
                            {
                                // Create touch
                                const Vector2 position = Vector2(GameActivityPointerAxes_getX(&pointer), static_cast<float32>(window->gameKitContext.GetScreen().GetHeight()) - GameActivityPointerAxes_getY(&pointer));
                                const Touch touch = Touch({
                                    .ID = TouchID(pointer.id),
                                    .type = TouchType::Press,
                                    .tapTime = TimePoint::Now(),
                                    .force = GameActivityPointerAxes_getPressure(&pointer),
                                    .position = position
                                });

                                window->touchManager.RegisterTouchPress(touch);
                                break;
                            }
                            case AMOTION_EVENT_ACTION_MOVE:
                            {
                                const Vector2 position = Vector2(GameActivityPointerAxes_getX(&pointer), static_cast<float32>(window->gameKitContext.GetScreen().GetHeight()) - GameActivityPointerAxes_getY(&pointer));
                                window->touchManager.RegisterTouchMove(TouchID(pointer.id), position);
                                break;
                            }
                            case AMOTION_EVENT_ACTION_CANCEL:
                            case AMOTION_EVENT_ACTION_UP:
                            {
                                window->touchManager.RegisterTouchRelease(TouchID(pointer.id));
                                break;
                            }
                            default:
                            {
                                continue;
                            }
                        }
                    }
                }
            }
            case APP_CMD_DESTROY:
            {
                window->Close();
                break;
            }
            default:
            {
                break;
            }
        }
    }

}