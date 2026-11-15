//
// Created by Nikolay Kanchevski on 10.10.25.
//

#pragma once

#include "FileOutputWizard.h"

namespace SierraEngine
{

    struct AssetSerializeWizardCreateInfo
    {
        std::span<const std::string_view> inputFileExtensions = { };
        const std::filesystem::path* inputFilePath = nullptr;

        std::string_view outputFileExtension = { };
        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class AssetSerializeWizard : public FileOutputWizard
    {
    public:
        /* --- CONSTANTS --- */
        constexpr static std::string_view BLOB_OUTPUT_FILE_EXTENSION = ".blob";

        /* --- COPY SEMANTICS --- */
        AssetSerializeWizard(const AssetSerializeWizard&) = delete;
        AssetSerializeWizard& operator=(const AssetSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        AssetSerializeWizard(AssetSerializeWizard&&) = delete;
        AssetSerializeWizard& operator=(AssetSerializeWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        ~AssetSerializeWizard() noexcept override = default;

    protected:
        /* --- CONSTANTS --- */
        constexpr static ImGuiTreeNodeFlags MENU_TREE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

        /* --- CONSTRUCTORS --- */
        explicit AssetSerializeWizard(const AssetSerializeWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        void DefaultMetadata(AssetMetadata& metadata, std::string_view name, const EditorPlatformContext& platformContext) const noexcept;
        void DrawMetadataMenu(AssetMetadata& metadata) const noexcept;

    };
}