//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "WindowsContext.h"

#include <objbase.h>
#include <commdlg.h>
#include <shellapi.h>
#include <CommCtrl.h>

namespace Sierra
{

    namespace
    {
        constexpr DWORD MAX_USER_NAME_LENGTH = 256 + 1;
        constexpr size MAX_DIALOG_PATHS_LENGTH = 32 * 1024;

        constexpr int ACCEPT_BUTTON_ID = 100;
        constexpr int REJECT_BUTTON_ID = 101;

        [[nodiscard]] std::string_view StripExtensionDot(std::string_view extension) noexcept
        {
            if (extension.starts_with('.')) extension.remove_prefix(1);
            return extension;
        }

        // Builds a filter, which is a name and a pattern (such as "*.png;*.jpg"), each ended by a null character, with one more at the very end
        [[nodiscard]] std::wstring CreateDialogFilter(const std::span<const std::string_view> extensions)
        {
            std::wstring pattern = { };
            for (const std::string_view extension : extensions)
            {
                if (StripExtensionDot(extension).empty()) continue;

                if (!pattern.empty()) pattern += L';';
                pattern += L"*." + Win32Context::UTF8ToWideString(StripExtensionDot(extension));
            }

            // NOTE: Last null character is the one c_str() ends the string with
            std::wstring filter = pattern.empty() ? L"All Files" : L"Supported Files";
            filter += L'\0';
            filter += pattern.empty() ? L"*.*" : pattern;
            filter += L'\0';

            return filter;
        }

        [[nodiscard]] std::wstring GetDialogDirectory(const Path* directoryPath)
        {
            if (directoryPath == nullptr) return { };
            return directoryPath->GetNormalizedPath().GetStandardPath().native();
        }

        [[nodiscard]] PCWSTR AlertSeverityToIcon(const AlertSeverity severity) noexcept
        {
            switch (severity)
            {
                case AlertSeverity::Info:
                case AlertSeverity::Question:   return TD_INFORMATION_ICON;
                case AlertSeverity::Warning:    return TD_WARNING_ICON;
                case AlertSeverity::Error:      return TD_ERROR_ICON;
            }

            return nullptr;
        }
    }

    /* --- CONSTRUCTORS --- */

    WindowsContext::WindowsContext()
        : win32Context({ .hInstance = GetModuleHandleW(nullptr) }), windowManager({ .win32Context = win32Context })
    {

    }

    /* --- POLLING METHODS --- */

    bool WindowsContext::OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept
    {
        const std::wstring title = Win32Context::UTF8ToWideString(openInfo.title);
        const std::wstring message = Win32Context::UTF8ToWideString(openInfo.message);
        const std::wstring acceptButtonText = Win32Context::UTF8ToWideString(openInfo.acceptButton.text);
        const std::wstring rejectButtonText = Win32Context::UTF8ToWideString(openInfo.rejectButton.text);

        const std::array<TASKDIALOG_BUTTON, 2> buttons
        {{
            { .nButtonID = ACCEPT_BUTTON_ID, .pszButtonText = acceptButtonText.c_str() },
            { .nButtonID = REJECT_BUTTON_ID, .pszButtonText = rejectButtonText.c_str() }
        }};

        TASKDIALOGCONFIG config = { };
        config.cbSize = sizeof(TASKDIALOGCONFIG);
        config.hwndParent = GetActiveWindow();
        config.dwFlags = TDF_ALLOW_DIALOG_CANCELLATION | TDF_POSITION_RELATIVE_TO_WINDOW;
        config.pszWindowTitle = title.c_str();
        config.pszMainIcon = AlertSeverityToIcon(openInfo.severity);
        config.pszContent = message.empty() ? nullptr : message.c_str();
        config.cButtons = rejectButtonText.empty() ? 1 : static_cast<UINT>(buttons.size());
        config.pButtons = buttons.data();
        config.nDefaultButton = ACCEPT_BUTTON_ID;

        // NOTE: Task dialogs come from version 6 of the common controls, which executables get, as Windows.cmake embeds a manifest requesting it
        int pressedButtonID = 0;
        const bool accepted = SUCCEEDED(TaskDialogIndirect(&config, &pressedButtonID, nullptr, nullptr)) && pressedButtonID == ACCEPT_BUTTON_ID;

        const AlertButton& pressedButton = accepted ? openInfo.acceptButton : openInfo.rejectButton;
        if (pressedButton.Callback != nullptr) pressedButton.Callback();

        return accepted;
    }

    std::vector<Path> WindowsContext::OpenPathSelectionDialog(const PathSelectionDialogOpenInfo& openInfo) const noexcept
    {
        // NOTE: Only files can be picked (as on Linux), and buttonText is not applied, as the dialog's buttons cannot be relabeled
        const std::wstring filter = CreateDialogFilter(openInfo.allowedFileExtensions);
        const std::wstring title = Win32Context::UTF8ToWideString(openInfo.message);
        const std::wstring initialDirectory = GetDialogDirectory(openInfo.directoryPath);

        std::wstring pathBuffer(MAX_DIALOG_PATHS_LENGTH, L'\0');

        OPENFILENAMEW dialogInfo = { };
        dialogInfo.lStructSize = sizeof(OPENFILENAMEW);
        dialogInfo.hwndOwner = GetActiveWindow();
        dialogInfo.lpstrFilter = filter.c_str();
        dialogInfo.lpstrFile = pathBuffer.data();
        dialogInfo.nMaxFile = static_cast<DWORD>(pathBuffer.size());
        dialogInfo.lpstrInitialDir = initialDirectory.empty() ? nullptr : initialDirectory.c_str();
        dialogInfo.lpstrTitle = title.empty() ? nullptr : title.c_str();
        dialogInfo.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | (openInfo.allowMultiplePaths ? OFN_ALLOWMULTISELECT : 0);

        // NOTE: Cancelling the dialog is reported as a failure
        if (!GetOpenFileNameW(&dialogInfo)) return { };

        // A single file is written as its path, while multiple ones are written as their directory, followed by their names, each ended by a null character (with one more at the very end)
        const std::wstring_view firstEntry = pathBuffer.c_str();
        const wchar_t* entry = pathBuffer.data() + firstEntry.size() + 1;
        if (*entry == L'\0') return { Path(std::filesystem::path(firstEntry)) };

        const std::filesystem::path directoryPath = firstEntry;
        std::vector<Path> paths = { };
        for (; *entry != L'\0'; entry += std::wstring_view(entry).size() + 1)
        {
            paths.emplace_back(directoryPath / entry);
        }

        return paths;
    }

    std::optional<Path> WindowsContext::OpenPathSaveDialog(const PathSaveDialogOpenInfo& openInfo) const noexcept
    {
        // NOTE: buttonText is not applied, as the dialog's buttons cannot be relabeled, and neither is showExtension, as whether extensions are shown is a setting of the user (made in File Explorer)
        const std::wstring filter = CreateDialogFilter(openInfo.allowedFileExtensions);
        const std::wstring title = Win32Context::UTF8ToWideString(openInfo.message);
        const std::wstring initialDirectory = GetDialogDirectory(openInfo.directoryPath);

        // Extension is appended to names typed without one, which is the first allowed one, as on the other platforms
        const std::wstring defaultExtension = openInfo.allowedFileExtensions.empty() ? std::wstring() : Win32Context::UTF8ToWideString(StripExtensionDot(openInfo.allowedFileExtensions.front()));

        // Name the dialog suggests is written into the buffer, which the chosen path is then written over
        std::wstring pathBuffer = Win32Context::UTF8ToWideString(openInfo.pathName);
        pathBuffer.resize(MAX_DIALOG_PATHS_LENGTH, L'\0');

        OPENFILENAMEW dialogInfo = { };
        dialogInfo.lStructSize = sizeof(OPENFILENAMEW);
        dialogInfo.hwndOwner = GetActiveWindow();
        dialogInfo.lpstrFilter = filter.c_str();
        dialogInfo.lpstrFile = pathBuffer.data();
        dialogInfo.nMaxFile = static_cast<DWORD>(pathBuffer.size());
        dialogInfo.lpstrInitialDir = initialDirectory.empty() ? nullptr : initialDirectory.c_str();
        dialogInfo.lpstrTitle = title.empty() ? nullptr : title.c_str();
        dialogInfo.lpstrDefExt = defaultExtension.empty() ? nullptr : defaultExtension.c_str();
        dialogInfo.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;

        // NOTE: Cancelling the dialog is reported as a failure
        if (!GetSaveFileNameW(&dialogInfo)) return std::nullopt;

        return Path(std::filesystem::path(pathBuffer.c_str()));
    }

    bool WindowsContext::OpenPath(const Path& path) const noexcept
    {
        // Path is opened with the application associated with it (or in File Explorer, if it is a directory). Results above 32 indicate success
        const HINSTANCE result = ShellExecuteW(nullptr, L"open", path.GetNormalizedPath().GetStandardPath().native().c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        return reinterpret_cast<INT_PTR>(result) > 32;
    }

    /* --- GETTER METHODS --- */

    std::string WindowsContext::GetUserName() const noexcept
    {
        std::array<WCHAR, MAX_USER_NAME_LENGTH> userName = { };
        DWORD userNameLength = static_cast<DWORD>(userName.size());

        // NOTE: Length includes the null terminator
        if (!GetUserNameW(userName.data(), &userNameLength) || userNameLength == 0) return { };
        return Win32Context::WideStringToUTF8({ userName.data(), userNameLength - 1 });
    }

    /* --- PRIVATE METHODS --- */

    void WindowsContext::Update()
    {
        win32Context.Update();
    }

}
