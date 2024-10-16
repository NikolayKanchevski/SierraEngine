//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the WindowsContext.h file is only allowed in Windows builds!"
#endif

#include "../PlatformContext.h"

#include "Win32Context.h"
#include "../../Files/Windows/Win32FileManager.h"

namespace Sierra
{

    class SIERRA_API WindowsContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowsContext();

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }

        [[nodiscard]] Screen& GetPrimaryScreen() noexcept override { return win32Context.GetPrimaryScreen(); }
        [[nodiscard]] Screen& GetWindowScreen(const Window& window) override;
        void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) override;

        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::Windows; }
        [[nodiscard]] Win32Context& GetWin32Context() { return win32Context; }

        /* --- COPY SEMANTICS --- */
        WindowsContext(const WindowsContext&) = delete;
        WindowsContext& operator=(const WindowsContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        WindowsContext(WindowsContext&&) = delete;
        WindowsContext& operator=(WindowsContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~WindowsContext() noexcept override = default;

    private:
        Win32Context win32Context;
        Win32FileManager fileManager;

        void Update() override;

    };

}
