//
// Created by Nikolay Kanchevski on 10.2.26.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the macOSEditorPlatformContext.h file is only allowed in macOS builds!"
#endif

#include "../EditorPlatformContext.h"

namespace SierraEngine
{

    class macOSEditorPlatformContext final : public EditorPlatformContext
    {
    public:
        /* --- CONSTRUCTORS --- */
        macOSEditorPlatformContext() noexcept;

        /* --- POLLING METHODS --- */
        bool OpenAlertDialog(const AlertDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::vector<std::filesystem::path> OpenFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept override;
        [[nodiscard]] std::optional<std::filesystem::path> OpenFileSaveDialog(const FileSaveDialogOpenInfo& openInfo) const noexcept override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string GetUserName() const noexcept override;

        /* --- COPY SEMANTICS --- */
        macOSEditorPlatformContext(const macOSEditorPlatformContext&) = delete;
        macOSEditorPlatformContext& operator=(const macOSEditorPlatformContext&) = delete;

        /* --- MOVE SEMANTICS --- */
        macOSEditorPlatformContext(macOSEditorPlatformContext&&) = delete;
        macOSEditorPlatformContext& operator=(macOSEditorPlatformContext&&) = delete;

        /* --- DESTRUCTOR --- */
        ~macOSEditorPlatformContext() noexcept override;

    private:

    };

}