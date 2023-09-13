//
// Created by Nikolay Kanchevski on 8.12.2023.
//

#include <Sierra.h>

//////////////////////////////////////////////////////////////////////////////////////////////
//////                    Simple Cross-Platform Multi-Windowing Test                    //////
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
    const uint8 TEST_WINDOW_COUNT = 2;
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

        // Log every window event in debug builds (Event::ToString() is debug-only)
        #if SR_DEBUG
            for (const auto &window : windows)
            {
                window->OnEvent<WindowMoveEvent>        ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowResizeEvent>      ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowFocusEvent>       ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowMinimizeEvent>    ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowMaximizeEvent>    ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
                window->OnEvent<WindowCloseEvent>       ([&window](const auto &event) { APP_INFO("{0} - {1}", window->GetTitle(), event.ToString()); return true; });
            }
        #endif
    }

    bool OnUpdate() override
    {
        bool allWindowsAreClosed = true;

        // Update all active windows
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

int32 main()
{
    // Create and run application
    SandboxApplication* application = new SandboxApplication({ .name = "Sandbox" });
    application->Run();

    delete(application);
    return 0;
}