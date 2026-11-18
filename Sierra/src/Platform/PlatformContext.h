//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "../Files/FileManager.h"
#include "../Windowing/WindowManager.h"

namespace Sierra
{

    enum class PlatformType : uint8
    {
        Windows,
        Linux,
        macOS,
        Android,
        iOS
    };

    enum class AlertSeverity : uint8
    {
        Info,
        Question,
        Warning,
        Error
    };

    using AlertButtonCallback = void(*)();
    struct AlertButton
    {
        std::string_view text = "Button";
        const AlertButtonCallback Callback = nullptr;
    };

    struct AlertDialogOpenInfo
    {
        std::string_view title = "Alert";
        std::string_view message = { };
        AlertSeverity severity = AlertSeverity::Info;

        AlertButton acceptButton = { .text = "Accept" };
        AlertButton rejectButton = { .text = "Reject" };
    };

    struct PathSelectionDialogOpenInfo
    {
        std::string_view message = { };
        std::string_view buttonText = "Open";
        const Path* directoryPath = nullptr;

        bool allowMultiplePaths = false;
        bool allowDirectories = true;
        bool allowFiles = true;

        std::span<const std::string_view> allowedFileExtensions = { };
    };

    struct PathSaveDialogOpenInfo
    {
        std::string_view message = { };
        std::string_view buttonText = { };
        const Path* directoryPath = nullptr;

        bool showExtension = true;

        std::string_view pathName = { };
        std::span<const std::string_view> allowedFileExtensions = { };
    };

    class SIERRA_API PlatformContext
    {
    public:
        /* --- POLLING METHODS --- */
        virtual bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept = 0;
        [[nodiscard]] virtual std::vector<Path> OpenPathSelectionDialog(const PathSelectionDialogOpenInfo& openInfo) const noexcept = 0;
        [[nodiscard]] virtual std::optional<Path> OpenPathSaveDialog(const PathSaveDialogOpenInfo& openInfo) const noexcept = 0;
        virtual bool OpenPath(const Path& path) const noexcept = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string GetUserName() const noexcept = 0;
        [[nodiscard]] virtual const WindowManager& GetWindowManager() const noexcept = 0;
        [[nodiscard]] virtual const FileManager& GetFileManager() const noexcept = 0;
        [[nodiscard]] virtual PlatformType GetType() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        PlatformContext(const PlatformContext&) = delete;
        PlatformContext& operator=(const PlatformContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        PlatformContext(PlatformContext&&) = delete;
        PlatformContext& operator=(PlatformContext&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~PlatformContext() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        PlatformContext() noexcept = default;

        friend class Application;
        virtual void Update() = 0;

    };

}
