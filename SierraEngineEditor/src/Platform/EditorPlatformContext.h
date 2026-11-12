//
// Created by Nikolay Kanchevski on 10.2.26.
//

#pragma once
#include "Sierra/Sierra.h"

namespace SierraEngine
{

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

    struct FileSelectDialogOpenInfo
    {
        std::string_view message = { };
        std::string_view buttonText = "Open";
        const std::filesystem::path* directoryPath = nullptr;

        bool allowMultipleFiles = false;
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

    class EditorPlatformContext
    {
    public:
        /* --- POLLING METHODS --- */
        virtual bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept = 0;
        [[nodiscard]] virtual std::vector<std::filesystem::path> OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept = 0;
        [[nodiscard]] virtual std::optional<std::filesystem::path> OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual std::string GetUserName() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        EditorPlatformContext(const EditorPlatformContext&) = delete;
        EditorPlatformContext& operator=(const EditorPlatformContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorPlatformContext(EditorPlatformContext&&) = delete;
        EditorPlatformContext& operator=(EditorPlatformContext&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~EditorPlatformContext() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        EditorPlatformContext() noexcept = default;

    };

}
