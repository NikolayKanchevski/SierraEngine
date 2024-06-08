//
// Created by Nikolay Kanchevski on 12/7/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the GameActivityContext.h file is only allowed in Android builds!"
#endif

#include "../../PlatformContext.h"

#include "GameActivityScreen.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Sierra
{

    struct GameActivityContextCreateInfo
    {
        android_app* app = nullptr;
    };

    class SIERRA_API GameActivityContext final
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] ANativeWindow* CreateWindow() const;
        void DestroyWindow(ANativeWindow* window) const;

        [[nodiscard]] bool IsEventQueueEmpty() const;
        uint32 PollNextEvent() const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const GameActivityScreen& GetPrimaryScreen() { return primaryScreen; };
        [[nodiscard]] android_app* GetApp() const { return app; }

        /* --- DESTRUCTOR --- */
        ~GameActivityContext();

    private:
        friend class AndroidContext;
        explicit GameActivityContext(const GameActivityContextCreateInfo &createInfo);

        android_app* app = nullptr;
        bool initialized = false;

        ANativeWindow* window = nullptr;
        GameActivityScreen primaryScreen;

        mutable std::queue<uint32> eventQueue;
        static void PoolCommand(android_app* app, int32 command);

    };

}
