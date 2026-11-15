    //
// Created by Nikolay Kanchevski on 3.09.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the macOSContext.h file is only allowed in macOS builds!"
#endif

#include "../PlatformContext.h"

#include "../../Files/Apple/FoundationFileManager.h"
#include "../../Windowing/macOS/CocoaWindowManager.h"

namespace Sierra
{

    class SIERRA_API macOSContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        macOSContext();

        /* --- GETTER METHODS --- */
        [[nodiscard]] const WindowManager& GetWindowManager() const noexcept override { return windowManager; }
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }

        /* --- POLLING METHODS --- */
        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::macOS; }

        /* --- COPY SEMANTICS --- */
        macOSContext(const macOSContext&) = delete;
        macOSContext& operator=(const macOSContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        macOSContext(macOSContext&&) = delete;
        macOSContext& operator=(macOSContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~macOSContext() noexcept override = default;

    private:
        CocoaWindowManager windowManager;
        FoundationFileManager fileManager;

        void Update() override { };

    };

}