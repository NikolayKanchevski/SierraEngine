//
// Created by Nikolay Kanchevski on 12/7/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the GameActivityContext.h file is only allowed in Android builds!"
#endif

#include "../PlatformContext.h"

#include "../../Windowing/Android/GameKitScreen.h"

namespace Sierra
{

    struct GameKitContextCreateInfo
    {
        android_app* app = nullptr;
    };

    class SIERRA_API GameKitContext final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using WindowCommandCallback = std::function<void(int32, void*)>;

        /* --- CONSTRUCTORS --- */
        explicit GameKitContext(const GameKitContextCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] ANativeWindow* CreateWindow(const WindowCommandCallback& Callback, void* userData = nullptr) const;
        void DestroyWindow(ANativeWindow* window) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const android_app* GetApp() const { return app; }
        [[nodiscard]] const android_input_buffer* GetCurrentInputBuffer() const { return currentInputBuffer; }

        [[nodiscard]] GameKitScreen& GetScreen() { return screen; }
        [[nodiscard]] const GameKitScreen& GetScreen() const { return screen; }

        /* --- COPY SEMANTICS --- */
        GameKitContext(const GameKitContext&) = delete;
        GameKitContext& operator=(const GameKitContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        GameKitContext(GameKitContext&&) = default;
        GameKitContext& operator=(GameKitContext&&) = default;

        /* --- DESTRUCTOR --- */
        ~GameKitContext() noexcept = default;

    private:
        android_app* app = nullptr;
        android_input_buffer* currentInputBuffer = nullptr;
        GameKitScreen screen;

        friend class AndroidContext;
        void Update();

        mutable std::queue<uint32> eventQueue;
        static void AppCmd(android_app* app, int32 command);

    };

}
