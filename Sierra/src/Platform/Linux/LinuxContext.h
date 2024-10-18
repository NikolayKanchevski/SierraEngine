//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the LinuxContext.h file is only allowed in Linux builds!"
#endif

#include "../PlatformContext.h"

#include "X11Context.h"
#include "../../Files/Unix/UnixFileManager.h"

namespace Sierra
{

    class SIERRA_API LinuxContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        LinuxContext();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }

        [[nodiscard]] Screen& GetPrimaryScreen() noexcept override { return x11Context.GetPrimaryScreen(); }
        [[nodiscard]] Screen& GetWindowScreen(const Window& window) override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) override;

        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::Linux; }
        [[nodiscard]] X11Context& GetX11Context() { return x11Context; }

        /* --- COPY SEMANTICS --- */
        LinuxContext(const LinuxContext&) = delete;
        LinuxContext& operator=(const LinuxContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        LinuxContext(LinuxContext&&) = delete;
        LinuxContext& operator=(LinuxContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~LinuxContext() noexcept override = default;

    private:
        X11Context x11Context;
        UnixFileManager fileManager;

        void Update() override;

    };

}
