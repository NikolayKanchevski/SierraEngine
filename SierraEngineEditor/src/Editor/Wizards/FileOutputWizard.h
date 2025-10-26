//
// Created by Nikolay Kanchevski on 22.10.25.
//

#pragma once

#include "../EditorWizard.h"

#include "../Widgets/ImGuiWidgets.h"

namespace SierraEngine
{

    struct FileOutputWizardCreateInfo
    {
        const Sierra::PlatformContext& platformContext;

        std::span<const std::string_view> inputFileExtensions = { };
        const std::filesystem::path* inputFilePath = nullptr;

        std::string_view outputFileExtension = { };
        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class FileOutputWizard : public virtual EditorWizard
    {
    public:
        /* --- COPY SEMANTICS --- */
        FileOutputWizard(const FileOutputWizard&) = delete;
        FileOutputWizard& operator=(const FileOutputWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileOutputWizard(FileOutputWizard&&) = delete;
        FileOutputWizard& operator=(FileOutputWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileOutputWizard() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit FileOutputWizard(const FileOutputWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        bool DrawFileOutputFooter(uint32& formatIndex, std::span<const ImGuiDropdownOption> formatOptions) noexcept;

        /* --- SETTER METHODS --- */
        virtual void SetOutputFilePath(const std::filesystem::path& inputFilePath) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::PlatformContext& GetPlatformContext() const noexcept { return platformContext; }

        [[nodiscard]] std::string_view GetOutputFileExtension() const noexcept { return outputFileExtension; }
        [[nodiscard]] const std::filesystem::path& GetOutputFilePath() const noexcept { return outputFilePath; }

    private:
        const Sierra::PlatformContext& platformContext;

        std::filesystem::path outputFilePath = { };
        std::string_view outputFileExtension = { };

    };

}