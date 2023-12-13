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
            windows[i] = GetWindowManager().CreateWindow({
                .title = title,
                .resizable = true,
                .maximize = false,
                .hide = false
            });
        }

        // Log every window event (will only log information if SIERRA_ENABLE_LOGGING is set)
        for (const auto &window : windows)
        {
            window->OnEvent<WindowMoveEvent>                                ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->OnEvent<WindowResizeEvent>                              ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->OnEvent<WindowFocusEvent>                               ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->OnEvent<WindowMinimizeEvent>                            ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->OnEvent<WindowMaximizeEvent>                            ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->OnEvent<WindowCloseEvent>                               ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });

            window->GetInputManager().OnEvent<KeyPressEvent>                ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->GetInputManager().OnEvent<KeyReleaseEvent>              ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->GetInputManager().OnEvent<MouseButtonPressEvent>        ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->GetInputManager().OnEvent<MouseButtonReleaseEvent>      ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->GetInputManager().OnEvent<MouseScrollEvent>             ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });

            window->GetCursorManager().OnEvent<CursorMoveEvent>             ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });

            window->GetTouchManager().OnEvent<TouchBeginEvent>              ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->GetTouchManager().OnEvent<TouchMoveEvent>               ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
            window->GetTouchManager().OnEvent<TouchEndEvent>                ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.GetLogString()); return true; });
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