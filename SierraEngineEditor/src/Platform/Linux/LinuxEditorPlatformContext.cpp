//
// Created by Nikolay Kanchevski on 10.2.26.
//

#include "LinuxEditorPlatformContext.h"

#include <pwd.h>
#include <unistd.h>
#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>
#include <QApplication>

namespace SierraEngine
{

    namespace
    {
        char applicationName[] = "Sierra Engine Editor";

        int argc = 1;
        char* argv[] = { applicationName, nullptr };

        [[nodiscard]] QMessageBox::Icon AlertSeverityToIcon(const AlertSeverity severity) noexcept
        {
            switch (severity)
            {
                case AlertSeverity::Info:     return QMessageBox::Icon::Information;
                case AlertSeverity::Question: return QMessageBox::Icon::Question;
                case AlertSeverity::Warning:  return QMessageBox::Icon::Warning;
                case AlertSeverity::Error:    return QMessageBox::Icon::Critical;
            }

            return QMessageBox::NoIcon;
        }

        [[nodiscard]] std::string_view StripExtensionDot(std::string_view extension)
        {
            if (extension.starts_with('.')) extension.remove_prefix(1);
            return extension;
        }

        [[nodiscard]] QString ExtensionsToNameFilter(const std::span<const std::string_view> extensions)
        {
            if (extensions.empty())
                return QStringLiteral("All Files (*)");

            QStringList patterns;
            for (const std::string_view extension : extensions)
            {
                patterns << QStringLiteral("*.") + QString::fromUtf8(StripExtensionDot(extension));
            }

            return QStringLiteral("Supported Files (")  + patterns.join(QLatin1Char(' ')) + QStringLiteral(") (")  + patterns.join(QLatin1Char(' ')) + QLatin1Char(')');
        }
    }

    /* --- CONSTRUCTORS --- */

    LinuxEditorPlatformContext::LinuxEditorPlatformContext() noexcept
        : application(argc, argv)
    {

    }

    /* --- POLLING METHODS --- */

    bool LinuxEditorPlatformContext::OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept
    {
        QMessageBox messageBox;
        messageBox.setIcon(AlertSeverityToIcon(openInfo.severity));
        messageBox.setWindowTitle(QString::fromUtf8(openInfo.title));
        messageBox.setText(QString::fromUtf8(openInfo.message));

        QPushButton* acceptButton = messageBox.addButton(QString::fromUtf8(openInfo.acceptButton.text), QMessageBox::AcceptRole);
        QPushButton* rejectButton = messageBox.addButton(QString::fromUtf8(openInfo.rejectButton.text), QMessageBox::RejectRole);

        messageBox.setDefaultButton(acceptButton);
        messageBox.setEscapeButton(rejectButton);

        messageBox.exec();

        const AlertButton& pressedButton = messageBox.clickedButton() == acceptButton ? openInfo.acceptButton : openInfo.rejectButton;
        if (pressedButton.Callback != nullptr) pressedButton.Callback();

        return messageBox.clickedButton() == acceptButton;
    }

    std::vector<std::filesystem::path> LinuxEditorPlatformContext::OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept
    {
        QFileDialog dialog;
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setFileMode(openInfo.allowMultipleFiles ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
        dialog.setLabelText(QFileDialog::Accept, QString::fromUtf8(openInfo.buttonText));
        dialog.setNameFilter(ExtensionsToNameFilter(openInfo.allowedFileExtensions));

        if (!openInfo.message.empty()) dialog.setWindowTitle(QString::fromUtf8(openInfo.message));
        if (openInfo.directoryPath != nullptr) dialog.setDirectory(QString::fromUtf8(openInfo.directoryPath->string()));

        if (dialog.exec() != QDialog::Accepted)
        {
            return { };
        }

        const QStringList selectedFiles = dialog.selectedFiles();
        if (selectedFiles.isEmpty()) return { };

        std::vector<std::filesystem::path> filePaths = { };
        filePaths.reserve(selectedFiles.size());

        for (const QString& filePath : selectedFiles)
        {
            filePaths.emplace_back(filePath.toStdString());
        }

        return filePaths;
    }

    std::optional<std::filesystem::path> LinuxEditorPlatformContext::OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept
    {
        QFileDialog dialog;
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setNameFilter(ExtensionsToNameFilter(openInfo.allowedFileExtensions));

        if (!openInfo.message.empty()) dialog.setWindowTitle(QString::fromUtf8(openInfo.message));
        if (!openInfo.buttonText.empty()) dialog.setLabelText(QFileDialog::Accept, QString::fromUtf8(openInfo.buttonText));
        if (openInfo.directoryPath != nullptr) dialog.setDirectory(QString::fromUtf8(openInfo.directoryPath->string()));

        if (!openInfo.fileName.empty()) dialog.selectFile(QString::fromUtf8(openInfo.fileName));
        if (!openInfo.allowedFileExtensions.empty())
        {
            dialog.setDefaultSuffix(QString::fromUtf8(StripExtensionDot(openInfo.allowedFileExtensions.front())));
        }

        if (dialog.exec() != QDialog::Accepted) return std::nullopt;

        const QStringList selectedFiles = dialog.selectedFiles();
        if (selectedFiles.size() != 1) return std::nullopt;

        return std::filesystem::path(selectedFiles.first().toStdU16String());
    }

    /* --- GETTER METHODS --- */

    std::string LinuxEditorPlatformContext::GetUserName() const noexcept
    {
        const uid_t uid = geteuid();
        if (const passwd* pw = getpwuid(uid); pw != nullptr)
        {
            return std::string(pw->pw_name);
        }

        return { };
    }

    /* --- DESTRUCTOR --- */

    LinuxEditorPlatformContext::~LinuxEditorPlatformContext() noexcept
    {
        application.quit();
    }
}
