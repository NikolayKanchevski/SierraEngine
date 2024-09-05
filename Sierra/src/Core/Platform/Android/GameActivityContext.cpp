//
// Created by Nikolay Kanchevski on 12/7/23.
//

#include "GameActivityContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    GameActivityContext::GameActivityContext(const GameActivityContextCreateInfo& createInfo)
        : app(createInfo.app), primaryScreen({ .gameActivity = app->activity, .configuration = app->config })
    {
        // Set up app data
        app->userData = this;
        app->onAppCmd = PoolCommand;

        // Allow event polling
        android_app_set_key_event_filter(app, nullptr);
        android_app_set_motion_event_filter(app, nullptr);

        while (true)
        {
            // Process system events
            int events;
            android_poll_source* source;
            while (ALooper_pollAll(0, nullptr, &events, reinterpret_cast<void**>(&source)) >= 0)
            {
                if (source != nullptr) source->process(app, source);
            }

            // Process input
            auto inputBuffer = android_app_swap_input_buffers(app);
            if (inputBuffer != nullptr)
            {
                android_app_clear_motion_events(inputBuffer);
                android_app_clear_motion_events(inputBuffer);
            }

            // Now that Android has loaded a window, it is up to GameActivityWindow::Update() to poll events
            if (initialized)
            {
                window = app->window;
                break;
            }
        }
    }

    /* --- POLLING METHODS --- */


    ANativeWindow* GameActivityContext::CreateWindow() const
    {
        return window;
    }

    void GameActivityContext::DestroyWindow(ANativeWindow*) const
    {

    }

    bool GameActivityContext::IsEventQueueEmpty() const
    {
        return eventQueue.empty();
    }

    uint32 GameActivityContext::PollNextEvent() const
    {
        uint32 event = eventQueue.front();
        eventQueue.pop();
        return event;
    }

    /* --- DESTRUCTOR --- */

    GameActivityContext::~GameActivityContext()
    {
        window = nullptr;
        app = nullptr;
    }

    /* --- PRIVATE METHODS --- */

    void GameActivityContext::PoolCommand(android_app* app, const int32 command)
    {
        GameActivityContext* context = reinterpret_cast<GameActivityContext*>(app->userData);
        if (command == APP_CMD_INIT_WINDOW) context->initialized = true;
        else context->eventQueue.push(command);
    }

}