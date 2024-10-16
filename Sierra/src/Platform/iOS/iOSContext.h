//
// Created by Nikolay Kanchevski on 26.09.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the iOSContext.h file is only allowed in iOS builds!"
#endif

#include "../PlatformContext.h"

#include "UIKitContext.h"
#include "../../Files/Apple/FoundationFileManager.h"

namespace Sierra
{

    class SIERRA_API iOSContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        iOSContext();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }

        [[nodiscard]] Screen& GetPrimaryScreen() noexcept override { return uiKitContext.GetScreen(); }
        [[nodiscard]] Screen& GetWindowScreen(const Window& window) override { return uiKitContext.GetScreen(); }
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) override { Predicate(uiKitContext.GetScreen()); }

        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::iOS; }
        [[nodiscard]] UIKitContext& GetUIKitContext() { return uiKitContext; }

        /* --- COPY SEMANTICS --- */
        iOSContext(const iOSContext&) = delete;
        iOSContext& operator=(const iOSContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        iOSContext(iOSContext&&) = delete;
        iOSContext& operator=(iOSContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~iOSContext() noexcept override = default;

    private:
        UIKitContext uiKitContext;
        FoundationFileManager fileManager;

    };

}
