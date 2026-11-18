//
// Created by Nikolay Kanchevski on 9.1.23.
//

#include "LinuxContext.h"

#undef None
#undef Unsorted
#undef signals

#include <QMessageBox>
#include <QPushButton>
#include <QFileDialog>

#include <pwd.h>
#include <gio/gio.h>

#include "../../Windowing/Linux/Wayland/WaylandContext.h"
#include "../../Windowing/Linux/Wayland/WaylandWindowManager.h"
#include "../../Windowing/Linux/X11/X11Context.h"
#include "../../Windowing/Linux/X11/X11WindowManager.h"

namespace Sierra
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

    LinuxContext::LinuxContext()
        : application(argc, argv)
    {
        // Wayland is preferred, when the session runs on it, and X11 is used otherwise (Wayland sessions also provide it through XWayland)
        if (std::getenv("WAYLAND_DISPLAY") != nullptr)
        {
            if (wl_display* waylandDisplay = wl_display_connect(nullptr); waylandDisplay != nullptr)
            {
                try
                {
                    WaylandContext& waylandContext = windowingContext.emplace<WaylandContext>(WaylandContextCreateInfo { .display = waylandDisplay });
                    windowManager = std::make_unique<WaylandWindowManager>(WaylandWindowManagerCreateInfo { .waylandContext = waylandContext });
                    return;
                }
                catch (const UnsupportedFeatureError&)
                {
                    windowingContext.emplace<std::monostate>();
                }
            }
        }

        XInitThreads();
        Display* x11Display = XOpenDisplay(nullptr);
        SR_THROW_IF(x11Display == nullptr, UnsupportedFeatureError("Cannot create Linux platform context, as neither a Wayland compositor, nor an X11 server could be connected to"));

        X11Context& x11Context = windowingContext.emplace<X11Context>(X11ContextCreateInfo { .display = x11Display });
        windowManager = std::make_unique<X11WindowManager>(X11WindowManagerCreateInfo { .x11Context = x11Context });
    }

    /* --- POLLING METHODS --- */

    bool LinuxContext::OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept
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

    std::vector<Path> LinuxContext::OpenPathSelectionDialog(const PathSelectionDialogOpenInfo& openInfo) const noexcept
    {
        QFileDialog dialog;
        dialog.setAcceptMode(QFileDialog::AcceptOpen);
        dialog.setFileMode(openInfo.allowMultiplePaths ? QFileDialog::ExistingFiles : QFileDialog::ExistingFile);
        dialog.setLabelText(QFileDialog::Accept, QString::fromUtf8(openInfo.buttonText));
        dialog.setNameFilter(ExtensionsToNameFilter(openInfo.allowedFileExtensions));

        if (!openInfo.message.empty()) dialog.setWindowTitle(QString::fromUtf8(openInfo.message));
        if (openInfo.directoryPath != nullptr) dialog.setDirectory(QString::fromUtf8(openInfo.directoryPath->GetString()));

        if (dialog.exec() != QDialog::Accepted)
        {
            return { };
        }

        const QStringList selectedFiles = dialog.selectedFiles();
        if (selectedFiles.isEmpty()) return { };

        std::vector<Path> filePaths = { };
        filePaths.reserve(selectedFiles.size());

        for (const QString& filePath : selectedFiles)
        {
            filePaths.emplace_back(filePath.toStdString());
        }

        return filePaths;
    }

    std::optional<Path> LinuxContext::OpenPathSaveDialog(const PathSaveDialogOpenInfo& openInfo) const noexcept
    {
        QFileDialog dialog;
        dialog.setAcceptMode(QFileDialog::AcceptSave);
        dialog.setFileMode(QFileDialog::AnyFile);
        dialog.setNameFilter(ExtensionsToNameFilter(openInfo.allowedFileExtensions));

        if (!openInfo.message.empty()) dialog.setWindowTitle(QString::fromUtf8(openInfo.message));
        if (!openInfo.buttonText.empty()) dialog.setLabelText(QFileDialog::Accept, QString::fromUtf8(openInfo.buttonText));
        if (openInfo.directoryPath != nullptr) dialog.setDirectory(QString::fromUtf8(openInfo.directoryPath->GetString()));

        if (!openInfo.pathName.empty()) dialog.selectFile(QString::fromUtf8(openInfo.pathName));
        if (!openInfo.allowedFileExtensions.empty())
        {
            dialog.setDefaultSuffix(QString::fromUtf8(StripExtensionDot(openInfo.allowedFileExtensions.front())));
        }

        if (dialog.exec() != QDialog::Accepted) return std::nullopt;
        const QStringList selectedFiles = dialog.selectedFiles();

        if (selectedFiles.empty())
            return std::nullopt;

        return Path(selectedFiles.first().toStdU16String());
    }

    bool LinuxContext::OpenPath(const Path& path) const noexcept
    {
        const std::string pathString = path;
        GFile* file = g_file_new_for_path(pathString.c_str());

        GError* error = nullptr;
        const bool success = g_app_info_launch_default_for_uri(g_file_get_uri(file), nullptr, &error);

        if (error)
            g_error_free(error);

        g_object_unref(file);
        return success;
    }

    /* --- GETTER METHODS --- */

    std::string LinuxContext::GetUserName() const noexcept
    {
        const uid_t uid = geteuid();
        if (const passwd* pw = getpwuid(uid); pw != nullptr)
        {
            return std::string(pw->pw_name);
        }

        return { };
    }


    /* --- PRIVATE METHODS --- */

    void LinuxContext::Update()
    {
        if (WaylandContext* waylandContext = std::get_if<WaylandContext>(&windowingContext); waylandContext != nullptr) waylandContext->Update();
        else std::get<X11Context>(windowingContext).Update();
    }

}