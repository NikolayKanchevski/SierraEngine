//
// Created by Nikolay Kanchevski on 22.10.25.
//

#pragma once

#include "../EditorWizard.h"

namespace SierraEngine
{

    struct FileInputWizardCreateInfo
    {
        const Sierra::PlatformContext& platformContext;

        std::span<const std::string_view> inputFileExtensions = { };
        const std::filesystem::path* inputFilePath = nullptr;

        std::string_view outputFileExtension = { };
        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class FileInputWizard : public virtual EditorWizard
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
        void DrawFileInputHeader() noexcept;

        /* --- SETTER METHODS --- */
        virtual void SetInputFilePath(const std::filesystem::path& inputFilePath) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::PlatformContext& GetPlatformContext() const noexcept { return *platformContext; }

        [[nodiscard]] std::span<const std::string_view> GetInputFileExtensions() const noexcept { return inputFileExtensions; }
        [[nodiscard]] const std::filesystem::path& GetInputFilePath() const noexcept { return inputFilePath; }

        /* --- MOVE SEMANTICS --- */
        FileInputWizard(FileInputWizard&&) noexcept = default;
        FileInputWizard& operator=(FileInputWizard&&) noexcept = default;

    private:
        const Sierra::PlatformContext* platformContext;

        std::filesystem::path inputFilePath = { };
        std::span<const std::string_view> inputFileExtensions = { };

    };

}