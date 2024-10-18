//
// Created by Nikolay Kanchevski on 30.11.23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the AndroidContext.h file is only allowed in Android builds!"
#endif

#include "../PlatformContext.h"

#include "GameKitContext.h"

namespace Sierra
{

    class SIERRA_API AndroidContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        AndroidContext();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return *fileManager; }

        [[nodiscard]] Screen& GetPrimaryScreen() noexcept override { return gameKitContext.GetScreen(); }
        [[nodiscard]] Screen& GetWindowScreen(const Window& window) override { return gameKitContext.GetScreen(); }
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) override { Predicate(gameKitContext.GetScreen()); }

        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::Android; }
        [[nodiscard]] GameKitContext& GetGameKitContext() { return gameKitContext; }

        /* --- COPY SEMANTICS --- */
        AndroidContext(const AndroidContext&) = delete;
        AndroidContext& operator=(const AndroidContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        AndroidContext(AndroidContext&&) = delete;
        AndroidContext& operator=(AndroidContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~AndroidContext() noexcept override = default;

    private:
        GameKitContext gameKitContext;
        FileManager* fileManager = nullptr; // TODO:

        inline static android_app* app = nullptr;
        friend void ::android_main(android_app*);

    };

}