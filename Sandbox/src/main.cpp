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
    #if !SR_PLATFORM_MOBILE
        const uint8 TEST_WINDOW_COUNT = 2;
    #else
        const uint8 TEST_WINDOW_COUNT = 1;
    #endif
    std::vector<UniquePtr<Window>> windows;

    void OnStart() override
    {
        // Create windows
        windows.resize(TEST_WINDOW_COUNT);
        for (uint32 i = 0; i < TEST_WINDOW_COUNT; i++)
        {
            const String title = "Window #" + std::to_string(i);
            windows[i] = Window::Create({
                .platformInstance = GetPlatformInstance(),
                .title = title,
                .resizable = true,
                .maximize = false,
                .hide = false
            });
        }

        // Log every window event (Event::ToString() is available in logging-enabled builds only)
        #if SR_ENABLE_LOGGING
            for (const auto &window : windows)
            {
                window->OnEvent<WindowMoveEvent>                                ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowResizeEvent>                              ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowFocusEvent>                               ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowMinimizeEvent>                            ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowMaximizeEvent>                            ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowCloseEvent>                               ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });

                window->GetInputManager().OnEvent<KeyPressEvent>                ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->GetInputManager().OnEvent<KeyReleaseEvent>              ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->GetInputManager().OnEvent<MouseButtonPressEvent>        ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->GetInputManager().OnEvent<MouseButtonReleaseEvent>      ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->GetInputManager().OnEvent<MouseScrollEvent>             ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });

                window->GetCursorManager().OnEvent<CursorMoveEvent>             ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });

                window->GetTouchManager().OnEvent<TouchBeginEvent>              ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->GetTouchManager().OnEvent<TouchMoveEvent>               ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->GetTouchManager().OnEvent<TouchEndEvent>                ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
            }
        #endif
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

        if (windows[0]->GetInputManager().IsKeyCombinationPressed(Key::LeftControl, Key::LeftShift, Key::R))
            printf("YES\n");

        return allWindowsAreClosed;
    }

};

int32 main()
{
    // Create and run application
    SandboxApplication* application = new SandboxApplication({ .name = "Sandbox" });
    application->Run();

    delete(application);
    return 0;
}
