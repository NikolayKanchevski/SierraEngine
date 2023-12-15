//
// Created by Nikolay Kanchevski on 12/14/23.
//

#include "GameActivityWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GameActivityWindow::GameActivityWindow(const GameActivityContext &gameActivityContext, const WindowCreateInfo &createInfo)
        : Window(createInfo), gameActivityContext(gameActivityContext), touchManager(gameActivityContext, { }), window(gameActivityContext.CreateWindow()), title(createInfo.title)
    {

    }

    /* --- POLLING METHODS --- */

    void GameActivityWindow::OnUpdate()
    {
        // Process system events
        int events;
        android_poll_source *source;
        while (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void**>(&source)) >= 0)
        {
            if (source != nullptr) source->process(gameActivityContext.GetApp(), source);
        }

        // Process window events
        while (!gameActivityContext.IsEventQueueEmpty())
        {
            const uint32 command = gameActivityContext.PollNextEvent();
            HandleCommand(command);
        }

        // Update touch manager
        touchManager.OnUpdate();
    }

    void GameActivityWindow::Minimize()
    {
        // Not applicable
    }

    void GameActivityWindow::Maximize()
    {
        // Not applicable
    }

    void GameActivityWindow::Show()
    {
        // Not applicable
    }

    void GameActivityWindow::Hide()
    {
        // Not applicable
    }

    void GameActivityWindow::Focus()
    {
        // Not applicable
    }

    void GameActivityWindow::Close()
    {
        closed = true;
        GetWindowCloseDispatcher().DispatchEvent();

        gameActivityContext.DestroyWindow(window);
        window = nullptr;
    }

    /* --- SETTER METHODS --- */

    void GameActivityWindow::SetTitle(const std::string &newTitle)
    {
        title = newTitle;
    }

    void GameActivityWindow::SetPosition(const Vector2Int &position)
    {
        // Not applicable
    }

    void GameActivityWindow::SetSize(const Vector2UInt &size)
    {
        // Not applicable
    }

    void GameActivityWindow::SetOpacity(float32 opacity)
    {
        // Not applicable
    }

    /* --- GETTER METHODS --- */

    const std::string& GameActivityWindow::GetTitle() const
    {
        return title;
    }

    Vector2Int GameActivityWindow::GetPosition() const
    {
        return { 0, 0 };
    }

    Vector2UInt GameActivityWindow::GetSize() const
    {
        return { ANativeWindow_getWidth(window), ANativeWindow_getHeight(window) };
    }

    Vector2UInt GameActivityWindow::GetFramebufferSize() const
    {
        return { ANativeWindow_getWidth(window), ANativeWindow_getHeight(window) };
    }

    float32 GameActivityWindow::GetOpacity() const
    {
        return 1.0f;
    }

    bool GameActivityWindow::IsClosed() const
    {
        return closed;
    }

    bool GameActivityWindow::IsMinimized() const
    {
        return minimized;
    }

    bool GameActivityWindow::IsMaximized() const
    {
        return !minimized;
    }

    bool GameActivityWindow::IsFocused() const
    {
        return !minimized;
    }

    bool GameActivityWindow::IsHidden() const
    {
        return minimized;
    }

    const Screen& GameActivityWindow::GetScreen() const
    {
        return gameActivityContext.GetPrimaryScreen();
    }

    TouchManager& GameActivityWindow::GetTouchManager()
    {
        return touchManager;
    }

    PlatformAPI GameActivityWindow::GetAPI() const
    {
        return PlatformAPI::GameActivity;
    }

    /* --- DESTRUCTOR --- */

    GameActivityWindow::~GameActivityWindow()
    {
        closed = true;
        GetWindowCloseDispatcher().DispatchEvent();

        gameActivityContext.DestroyWindow(window);
        window = nullptr;
    }

    /* --- PRIVATE METHODS --- */

    void GameActivityWindow::HandleCommand(const uint32 command)
    {
        switch (command)
        {
            case APP_CMD_GAINED_FOCUS:
            {
                minimized = false;
                GetWindowFocusDispatcher().DispatchEvent(true);
                break;
            }
            case APP_CMD_LOST_FOCUS:
            {
                minimized = true;
                GetWindowFocusDispatcher().DispatchEvent(false);
                break;
            }
            case APP_CMD_DESTROY:
            {
                Close();
                break;
            }
            default:
            {
                break;
            }
        }
    }

}