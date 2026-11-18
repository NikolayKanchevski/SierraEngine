//
// Created by Nikolay Kanchevski on 9.1.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the LinuxContext.h file is only allowed in Linux builds!"
#endif

#include <QApplication>

#include "../PlatformContext.h"

#include "../../Files/Unix/UnixFileManager.h"
#include "../../Windowing/Linux/X11/X11Context.h"
#include "../../Windowing/Linux/Wayland/WaylandContext.h"

namespace Sierra
{

    class SIERRA_API LinuxContext final : public PlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        LinuxContext();

        /* --- POLLING METHODS --- */
        bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::vector<Path> OpenPathSelectionDialog(const PathSelectionDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::optional<Path> OpenPathSaveDialog(const PathSaveDialogOpenInfo& openInfo) const noexcept override;
        bool OpenPath(const Path& path) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetUserName() const noexcept override;
        [[nodiscard]] const WindowManager& GetWindowManager() const noexcept override { return *windowManager; }
        [[nodiscard]] const FileManager& GetFileManager() const noexcept override { return fileManager; }
        [[nodiscard]] PlatformType GetType() const noexcept override { return PlatformType::Linux; }

        /* --- COPY SEMANTICS --- */
        LinuxContext(const LinuxContext&) = delete;
        LinuxContext& operator=(const LinuxContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        LinuxContext(LinuxContext&&) = delete;
        LinuxContext& operator=(LinuxContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~LinuxContext() noexcept override = default;

    private:
        QApplication application;
        std::variant<std::monostate, WaylandContext, X11Context> windowingContext = std::monostate();

        std::unique_ptr<WindowManager> windowManager = nullptr;
        UnixFileManager fileManager = { };

        void Update() override;

    };

}
