//
// Created by Nikolay Kanchevski on 10.2.26.
//

#pragma once

#include <QApplication>

#include "../EditorPlatformContext.h"

namespace SierraEngine
{

    class LinuxEditorPlatformContext final : public EditorPlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        LinuxEditorPlatformContext() noexcept;

        /* --- POLLING METHODS --- */
        bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::vector<std::filesystem::path> OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::optional<std::filesystem::path> OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetUserName() const noexcept override;

        /* --- COPY SEMANTICS --- */
        LinuxEditorPlatformContext(const LinuxEditorPlatformContext&) = delete;
        LinuxEditorPlatformContext& operator=(const LinuxEditorPlatformContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        LinuxEditorPlatformContext(LinuxEditorPlatformContext&&) = delete;
        LinuxEditorPlatformContext& operator=(LinuxEditorPlatformContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~LinuxEditorPlatformContext() noexcept override;

    private:
        QApplication application;

    };

}