    //
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "../Windowing/Screen.h"
#include "../Windowing/Window.h"
#include "../Files/FileManager.h"

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
        Warning,
        Error
    };

    struct AlertDialogOpenInfo
    {
        std::string_view title = "Alert";
        std::string_view message = { };
        AlertSeverity severity = AlertSeverity::Warning;

        std::string_view acceptButtonText = "OK";
        std::string_view declineButtonText = { };
    };

    struct FileSelectDialogOpenInfo
    {
        std::string_view message = { };
        std::string_view buttonText = { };
        const std::filesystem::path* directoryPath = nullptr;

        bool allowFiles = true;
        bool allowDirectories = false;
        bool allowMultipleSelection = false;
        std::span<const std::string_view> allowedFileExtensions = { };
    };

    struct FileSaveDialogOpenInfo
    {
        std::string_view message = { };
        std::string_view buttonText = { };
        const std::filesystem::path* directoryPath = nullptr;

        std::string_view fileName = { };
        std::span<const std::string_view> allowedFileExtensions = { };
    };

    class SIERRA_API PlatformContext
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using ScreenEnumerationPredicate = std::function<void(Screen&)>;

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::unique_ptr<Window> CreateWindow(const WindowCreateInfo& createInfo) const = 0;

        virtual bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const = 0;

        [[nodiscard]] std::optional<std::filesystem::path> OpenSingleFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept;
        [[nodiscard]] virtual std::vector<std::filesystem::path> OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept = 0;
        [[nodiscard]] virtual std::optional<std::filesystem::path> OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string_view GetUserName() const noexcept = 0;
        [[nodiscard]] virtual const FileManager& GetFileManager() const noexcept = 0;

        [[nodiscard]] virtual Screen& GetPrimaryScreen() noexcept = 0;
        [[nodiscard]] virtual Screen& GetWindowScreen(const Window& window) = 0;
        virtual void EnumerateScreens(const ScreenEnumerationPredicate& Predicate) = 0;

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
        virtual void Update() { };

    };

}
