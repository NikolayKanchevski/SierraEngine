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
#include "../../Windowing/Windows/Win32WindowManager.h"

namespace Sierra
{

    class SIERRA_API WindowsContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        WindowsContext();

        /* --- POLLING METHODS --- */
        bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::vector<Path> OpenPathSelectionDialog(const PathSelectionDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::optional<Path> OpenPathSaveDialog(const PathSaveDialogOpenInfo& openInfo) const noexcept override;
        bool OpenPath(const Path& path) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetUserName() const noexcept override;
        [[nodiscard]] const WindowManager& GetWindowManager() const noexcept override { return windowManager; }
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }
        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::Windows; }

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
        Win32WindowManager windowManager;
        Win32FileManager fileManager = { };

        void Update() override;

    };

}
