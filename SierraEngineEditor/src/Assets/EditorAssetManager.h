//
// Created by Nikolay Kanchevski on 9.10.25.
//

#pragma once

#include "../Core/ApplicationFrame.h"
#include "Shaders/Compilers/SlangShaderCompiler.h"

namespace SierraEngine
{

    struct EditorAssetManagerCreateInfo
    {
        const Sierra::Device& device;
        const Sierra::FileManager& fileManager;
    };

    struct BuiltInShaderLoadInfo
    {
        std::string_view name; // Name of the shader asset within the Resources folder
        const Sierra::Path& sourceFilePath; // Source to compile the shader from, if it exists (what it includes is looked up relative to it)
    };

    class EditorAssetManager final : public AssetManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorAssetManager(const EditorAssetManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update(const DrawFrame& drawFrame) override;

        void LoadProjectAssets(const ApplicationFrame& applicationFrame);
        [[nodiscard]] std::optional<ShaderID> LoadBuiltInShader(const BuiltInShaderLoadInfo& loadInfo);

        void ImportTexture(const ImportedTexture& importedTexture) override;
        void ImportMaterial(const ImportedMaterial& importedMaterial) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Sierra::Shader* GetShader(ShaderID ID, std::string_view entryPointName) const noexcept;

        /* --- COPY SEMANTICS --- */
        EditorAssetManager(const EditorAssetManager &) = delete;
        EditorAssetManager& operator=(const EditorAssetManager &) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorAssetManager(EditorAssetManager &&) noexcept = default;
        EditorAssetManager& operator=(EditorAssetManager &&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~EditorAssetManager() noexcept override = default;

    private:
        const Sierra::Device* device = nullptr;
        const Sierra::FileManager* fileManager = nullptr;

        struct ShaderEntryPointObject
        {
            std::string name = { };
            std::unique_ptr<Sierra::Shader> shader = nullptr;
        };

        struct LoadedShader
        {
            std::string name = { };
            Sierra::Path sourceFilePath = { };
            Sierra::Path assetFilePath = { };
            std::vector<ShaderEntryPointObject> entryPoints = { };
        };

        std::unordered_map<ShaderID::ValueType, LoadedShader> shaders = { };
        std::unique_ptr<SlangShaderCompiler> shaderCompiler = nullptr;

        [[nodiscard]] std::optional<ImportedShader> ImportShaderFiles(const Sierra::Path& assetFilePath) const;
        void CreateShaderEntryPoints(LoadedShader& shader, const ImportedShader& importedShader) const;

    };
}
