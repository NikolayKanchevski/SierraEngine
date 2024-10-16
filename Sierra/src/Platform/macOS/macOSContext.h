//
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the macOSContext.h file is only allowed in macOS builds!"
#endif

#include "../PlatformContext.h"

#include "CocoaContext.h"
#include "../../Files/Apple/FoundationFileManager.h"

namespace Sierra
{

    class SIERRA_API macOSContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        macOSContext();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }

        [[nodiscard]] Screen& GetPrimaryScreen() noexcept override { return cocoaContext.GetPrimaryScreen(); }
        [[nodiscard]] Screen& GetWindowScreen(const Window& window) override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) override;

        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::macOS; }
        [[nodiscard]] CocoaContext& GetCocoaContext() noexcept { return cocoaContext; }

        /* --- COPY SEMANTICS --- */
        macOSContext(const macOSContext&) = delete;
        macOSContext& operator=(const macOSContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        macOSContext(macOSContext&&) = delete;
        macOSContext& operator=(macOSContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~macOSContext() noexcept override = default;

    private:
        CocoaContext cocoaContext;
        FoundationFileManager fileManager;

        void Update() override;

    };

}