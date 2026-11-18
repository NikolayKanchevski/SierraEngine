//
// Created by Nikolay Kanchevski on 22.10.25.
//

#pragma once

#include "../Modal.h"

namespace SierraEngine
{

    struct FileInputWizardCreateInfo
    {
        std::span<const std::string_view> inputFileExtensions = { };
        const Sierra::Path* inputFilePath = nullptr;

        std::string_view outputFileExtension = { };
        const Sierra::Path* outputDirectoryPath = nullptr;
    };

    class FileInputWizard : public virtual Modal
    {
    public:
        /* --- COPY SEMANTICS --- */
        FileInputWizard(const FileInputWizard&) = delete;
        FileInputWizard& operator=(const FileInputWizard&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileInputWizard() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit FileInputWizard(const FileInputWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        void DrawFileInputHeader(const Sierra::PlatformContext& platformContext) noexcept;

        /* --- SETTER METHODS --- */
        virtual void SetInputFilePath(const Sierra::Path& inputFilePath) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::span<const std::string_view> GetInputFileExtensions() const noexcept { return inputFileExtensions; }
        [[nodiscard]] const Sierra::Path& GetInputFilePath() const noexcept { return inputFilePath; }

        /* --- MOVE SEMANTICS --- */
        FileInputWizard(FileInputWizard&&) noexcept = default;
        FileInputWizard& operator=(FileInputWizard&&) noexcept = default;

    private:
        Sierra::Path inputFilePath = { };
        std::span<const std::string_view> inputFileExtensions = { };

    };

}