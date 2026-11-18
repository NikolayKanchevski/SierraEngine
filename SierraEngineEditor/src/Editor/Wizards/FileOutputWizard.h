//
// Created by Nikolay Kanchevski on 22.10.25.
//

#pragma once

#include "../Modal.h"

#include "../Widgets/ImGuiWidgets.h"

namespace SierraEngine
{

    struct FileOutputWizardCreateInfo
    {
        std::span<const std::string_view> inputFileExtensions = { };
        const Sierra::Path* inputFilePath = nullptr;

        std::string_view outputFileExtension = { };
        const Sierra::Path* outputDirectoryPath = nullptr;
    };

    class FileOutputWizard : public virtual Modal
    {
    public:
        /* --- COPY SEMANTICS --- */
        FileOutputWizard(const FileOutputWizard&) = delete;
        FileOutputWizard& operator=(const FileOutputWizard&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileOutputWizard() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit FileOutputWizard(const FileOutputWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        bool DrawFileOutputFooter(const Sierra::PlatformContext& platformContext, uint32& formatIndex, std::span<const ImGuiDropdownOption> formatOptions) noexcept;

        /* --- SETTER METHODS --- */
        virtual void SetOutputFilePath(const Sierra::Path& inputFilePath) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetOutputFileExtension() const noexcept { return outputFileExtension; }
        [[nodiscard]] const Sierra::Path& GetOutputFilePath() const noexcept { return outputFilePath; }

        /* --- MOVE SEMANTICS --- */
        FileOutputWizard(FileOutputWizard&&) noexcept = default;
        FileOutputWizard& operator=(FileOutputWizard&&) noexcept = default;

    private:
        Sierra::Path outputFilePath = { };
        std::string_view outputFileExtension = { };

    };

}