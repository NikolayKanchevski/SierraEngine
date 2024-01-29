//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#include <Sierra.h>

//////////////////////////////////////////////////////////////////////////////////////////////
/////            Simple Cross-Platform Multi-Windowing & Input Handling Test            //////
//////////////////////////////////////////////////////////////////////////////////////////////
class SandboxApplication final : public Application
{
public:
    explicit SandboxApplication(const ApplicationCreateInfo &createInfo)
            : Application(createInfo)
    {
        APP_INFO("Application launched.");
    }

private:
    const uint8 WINDOW_COUNT = !SR_PLATFORM_MOBILE + 1;
    std::vector<std::unique_ptr<Window>> windows;

    void OnStart() override
    {
        // Create windows
        windows.resize(WINDOW_COUNT);
        for (uint32 i = 0; i < WINDOW_COUNT; i++)
        {
            const std::string title = "Window #" + std::to_string(i);
            windows[i] = GetWindowManager()->CreateWindow({
                .title = title,
                .resizable = true,
                .maximize = false,
                .hide = false
            });
        }

        // Log every window event (will only log information if SIERRA_ENABLE_LOGGING is set)
        for (const auto &window : windows)
        {
            window->OnEvent<WindowMoveEvent>                                ([&window](const auto &event) { APP_INFO("{0} - Window Move Event",     window->GetTitle()); return false; });
            window->OnEvent<WindowResizeEvent>                              ([&window](const auto &event) { APP_INFO("{0} - Window Resize Event",   window->GetTitle()); return false; });
            window->OnEvent<WindowFocusEvent>                               ([&window](const auto &event) { APP_INFO("{0} - Window Focus Event",    window->GetTitle()); return false; });
            window->OnEvent<WindowMinimizeEvent>                            ([&window](const auto &event) { APP_INFO("{0} - Window Minimize Event", window->GetTitle()); return false; });
            window->OnEvent<WindowMaximizeEvent>                            ([&window](const auto &event) { APP_INFO("{0} - Window Maximize Event", window->GetTitle()); return false; });
            window->OnEvent<WindowCloseEvent>                               ([&window](const auto &event) { APP_INFO("{0} - Window Close Event",    window->GetTitle()); return false; });

            window->GetInputManager().OnEvent<KeyPressEvent>                ([&window](const auto &event) { APP_INFO("{0} - Key Press Event - [{1}]",            window->GetTitle(), GetKeyName(event.GetKey()));                 return false; });
            window->GetInputManager().OnEvent<KeyReleaseEvent>              ([&window](const auto &event) { APP_INFO("{0} - Key Release Event - [{1}]",          window->GetTitle(), GetKeyName(event.GetKey()));                 return false; });
            window->GetInputManager().OnEvent<MouseButtonPressEvent>        ([&window](const auto &event) { APP_INFO("{0} - Mouse Button Press Event - [{1}]",   window->GetTitle(), GetMouseButtonName(event.GetMouseButton())); return false; });
            window->GetInputManager().OnEvent<MouseButtonReleaseEvent>      ([&window](const auto &event) { APP_INFO("{0} - Mouse Button Release Event - [{1}]", window->GetTitle(), GetMouseButtonName(event.GetMouseButton())); return false; });
            window->GetInputManager().OnEvent<MouseScrollEvent>             ([&window](const auto &event) { APP_INFO("{0} - Mouse Scroll Event",                 window->GetTitle());                                             return false; });

            window->GetCursorManager().OnEvent<CursorMoveEvent>             ([&window](const auto &event) { APP_INFO("{0} - Cursor Move Event", window->GetTitle()); return false; });

            window->GetTouchManager().OnEvent<TouchBeginEvent>              ([&window](const auto &event) { APP_INFO("{0} - Touch Begin Event", window->GetTitle()); return false; });
            window->GetTouchManager().OnEvent<TouchMoveEvent>               ([&window](const auto &event) { APP_INFO("{0} - Touch Move Event",  window->GetTitle()); return false; });
            window->GetTouchManager().OnEvent<TouchEndEvent>                ([&window](const auto &event) { APP_INFO("{0} - Touch End Event",   window->GetTitle()); return false; });
        }
    }

    bool OnUpdate(const TimeStep &timeStep) override
    {
        bool allWindowsAreClosed = true;

        // Update all active windows and check if they are closed
        for (const auto &window : windows)
        {
            if (!window->IsClosed())
            {
                window->OnUpdate();
                allWindowsAreClosed = false;
            }
        }

        return allWindowsAreClosed;
    }

};

int main()
{
    // Create and run application
    std::unique_ptr<SandboxApplication> application = std::make_unique<SandboxApplication>(ApplicationCreateInfo{ .name = "Sandbox" });
    application->Run();
    return 0;
}