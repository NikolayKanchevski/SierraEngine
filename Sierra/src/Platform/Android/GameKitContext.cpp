//
// Created by Nikolay Kanchevski on 12/7/23.
//

#include "GameKitContext.h"

namespace Sierra
{

    namespace
    {
        struct WindowEntry
        {
            ANativeWindow* window = nullptr;
            GameKitContext::WindowCommandCallback Callback = { };
            void* userData = nullptr;
        };
        std::vector<WindowEntry> windowMap = { };
    }

    /* --- CONSTRUCTORS --- */

    GameKitContext::GameKitContext(const GameKitContextCreateInfo& createInfo)
        : app(createInfo.app), screen({ .gameActivity = app->activity, .configuration = app->config })
    {
        SR_THROW_IF(createInfo.app == nullptr, InvalidValueError("Cannot create GameKit context, as specified app must not be nil"));

        // Set up app data
        app->userData = this;
        app->onAppCmd = AppCmd;

        // Allow event polling
        android_app_set_key_event_filter(app, nullptr);
        android_app_set_motion_event_filter(app, nullptr);

        // Wait until application has been initialized
        while (app->window == nullptr) Update();
    }

    /* --- POLLING METHODS --- */

    ANativeWindow* GameKitContext::CreateWindow(const WindowCommandCallback& Callback, void* const userData) const
    {
        ANativeWindow_acquire(app->window);
        ANativeWindow* const window = app->window;

        const WindowEntry windowEntry
        {
            .window = window,
            .Callback = Callback,
            .userData = userData
        };
        windowMap.emplace_back(windowEntry);

        return app->window;
    }

    void GameKitContext::DestroyWindow(ANativeWindow* const window) const
    {
        auto iterator = std::find_if(windowMap.begin(), windowMap.end(), [window](const WindowEntry& entry) -> bool { return entry.window == window; });
        if (iterator != windowMap.end()) windowMap.erase(iterator);

        ANativeWindow_release(window);
    }

    /* --- PRIVATE METHODS --- */

    void GameKitContext::Update()
    {
        // Process system events
        int events;
        android_poll_source* source = nullptr;
        while (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void**>(&source)) >= 0)
        {
            if (source != nullptr) source->process(app, source);
        }

        // Clear input data of last frame
        if (currentInputBuffer != nullptr)
        {
            android_app_clear_motion_events(currentInputBuffer);
            android_app_clear_key_events(currentInputBuffer);
        }

        // Process input
        currentInputBuffer = android_app_swap_input_buffers(app);
    }

    void GameKitContext::AppCmd(android_app* app, const int32 command)
    {
        GameKitContext* const context = reinterpret_cast<GameKitContext*>(app->userData);
        switch (command)
        {
            case APP_CMD_SAVE_STATE:
            case APP_CMD_CONTENT_RECT_CHANGED:
            {
                return;
            }
            case APP_CMD_DESTROY:
            {
                break; // TODO: SEE IF DESTR ARE RUN OTHERWISE USE THIS
            }
            default:
            {
                break;
            }
        }

        // Dispatch command to windows
        for (const WindowEntry& windowEntry : windowMap)
        {
            windowEntry.Callback(command, windowEntry.userData);
        }
    }

}