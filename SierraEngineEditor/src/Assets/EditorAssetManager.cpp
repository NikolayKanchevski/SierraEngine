//
// Created by Nikolay Kanchevski on 9.10.25.
//

#include "EditorAssetManager.h"

#include "../Editor/Wizards/AssetSerializeWizard.h"
#include "Textures/Importers/AutoTextureImporter.h"
#include "Shaders/Importers/AutoShaderImporter.h"
#include "Shaders/Serializers/YAMLShaderSerializer.h"

namespace SierraEngine
{

    const std::unordered_map<std::string_view, AssetType> ASSET_FILE_EXTENSION_MAP
    {
        { ".texture", AssetType::Texture },
        { ".material", AssetType::Material },
        { ".model", AssetType::Model },
    };

    namespace
    {
        constexpr std::string_view SHADER_FILE_EXTENSION = ".shader";
        constexpr std::string_view BUILT_IN_SHADER_DIRECTORY_NAME = "Shaders";
    }

    /* --- CONSTRUCTORS --- */

    EditorAssetManager::EditorAssetManager(const EditorAssetManagerCreateInfo& createInfo)
        : AssetManager({ }), device(&createInfo.device), fileManager(&createInfo.fileManager)
    {

    }

    /* --- POLLING METHODS --- */

    void EditorAssetManager::Update(const DrawFrame& drawFrame)
    {

    }

    void EditorAssetManager::LoadProjectAssets(const ApplicationFrame& frame)
    {
//        const Sierra::Path assetDirectoryPath = project.GetAssetPath();
//        fileManager.EnumerateDirectoryFiles(assetDirectoryPath, true, [this, &fileManager](const Sierra::Path& filePath) -> void
//        {
//            if (!filePath.has_extension())
//                return;
//
//            const std::string extension = filePath.extension().string();
//            if (const auto iterator = ASSET_FILE_EXTENSION_MAP.find(extension); iterator != ASSET_FILE_EXTENSION_MAP.end())
//            {
//                std::vector<uint8> assetData = fileManager.ReadFile(filePath.string());
//                switch (iterator->second)
//                {
//                    case AssetType::Texture:
//                    {
//                        const Sierra::Path blobFilePath = Sierra::Path(filePath).ChangeExtension(AssetSerializeWizard::BLOB_OUTPUT_FILE_EXTENSION);
//                        if (!fileManager.FileExists(blobFilePath))
//                        {
//                            APP_WARNING("Asset manager cannot load asset [{0}], as its corresponding memory [.blob] file [{1}] is missing", filePath.string(), blobFilePath.string());
//                            return;
//                        }
//
//                        std::vector<uint8> assetBlob = fileManager.ReadFile(blobFilePath);
//                        const SerializedTexture serializedTexture
//                        {
//                            .data = std::move(assetData),
//                            .blob = std::move(assetBlob)
//                        };
//
//                        const std::optional<ImportedTexture> importedTexture = AutoTextureImporter::Import({
//                            .serializedTexture = serializedTexture,
//                            .format = Sierra::ImageFormat::R8G8B8A8_UNorm
//                        });
//
//                        if (!importedTexture.has_value())
//                        {
//                            APP_WARNING("Asset manager could not load asset [{0}]", filePath.string());
//                            return;
//                        }
//
//                        ImportTexture(importedTexture.value());
//                        APP_INFO("Texture Imported - [{0}]", filePath.string());
//                        break;
//                    }
//                    case AssetType::Shader:
//                    case AssetType::Material:
//                    case AssetType::Model:
//                    case AssetType::Unknown:
//                    {
//                        return;
//                    }
//                }
//            }
//        });
    }

    std::optional<ShaderID> EditorAssetManager::LoadBuiltInShader(const BuiltInShaderLoadInfo& loadInfo)
    {
        LoadedShader shader
        {
            .name = std::string(loadInfo.name),
            .sourceFilePath = loadInfo.sourceFilePath,
            .assetFilePath = fileManager->GetResourcesDirectoryPath() / BUILT_IN_SHADER_DIRECTORY_NAME / SR_FORMAT("{0}{1}", loadInfo.name, SHADER_FILE_EXTENSION)
        };

        // The shader compiled the last time (if any) is used as-is if its source is missing (as is the case outside of the repository), or fails to compile
        std::optional<ImportedShader> importedShader = ImportShaderFiles(shader.assetFilePath);
        if (!importedShader.has_value())
        {
            APP_ERROR("Could not load built-in shader [{0}], as neither could it be compiled from [{1}], nor has it been compiled before", shader.name, shader.sourceFilePath.GetString());
            return std::nullopt;
        }

        CreateShaderEntryPoints(shader, *importedShader);

        const ShaderID ID = importedShader->ID;
        shaders.insert_or_assign(ID.GetValue(), std::move(shader));
        return ID;
    }

    void EditorAssetManager::ImportTexture(const ImportedTexture& importedTexture)
    {

    }

    void EditorAssetManager::ImportMaterial(const ImportedMaterial& importedMaterial)
    {

    }

    /* --- GETTER METHODS --- */

    const Sierra::Shader* EditorAssetManager::GetShader(const ShaderID ID, const std::string_view entryPointName) const noexcept
    {
        const auto iterator = shaders.find(ID.GetValue());
        if (iterator == shaders.end())
            return nullptr;

        for (const ShaderEntryPointObject& entryPoint : iterator->second.entryPoints)
        {
            if (entryPoint.name == entryPointName)
                return entryPoint.shader.get();
        }

        return nullptr;
    }

    /* --- PRIVATE METHODS --- */

    std::optional<ImportedShader> EditorAssetManager::ImportShaderFiles(const Sierra::Path& assetFilePath) const
    {
        SerializedShader serializedShader = { };
        if (!fileManager->IsFile(assetFilePath) || fileManager->ReadFile(assetFilePath, serializedShader.data).has_value())
            return std::nullopt;

        const Sierra::Path blobFilePath = Sierra::Path(assetFilePath).ChangeExtension(AssetSerializeWizard::BLOB_OUTPUT_FILE_EXTENSION);
        if (!fileManager->IsFile(blobFilePath) || fileManager->ReadFile(blobFilePath, serializedShader.blob).has_value())
        {
            APP_WARNING("Could not import shader [{0}], as its corresponding [{1}] file [{2}] could not be read", assetFilePath.GetString(), AssetSerializeWizard::BLOB_OUTPUT_FILE_EXTENSION, blobFilePath.GetString());
            return std::nullopt;
        }

        return AutoShaderImporter::Import({ .serializedShader = serializedShader, .backendType = device->GetBackendType() });
    }

    void EditorAssetManager::CreateShaderEntryPoints(LoadedShader& shader, const ImportedShader& importedShader) const
    {
        shader.entryPoints.reserve(importedShader.properties.entryPoints.size());

        const std::span<const uint8> buffer = importedShader.buffer;
        for (const ShaderEntryPointProperties& entryPointProperties : importedShader.properties.entryPoints)
        {
            const std::span<const uint8> code = buffer.subspan(entryPointProperties.codeOffset, entryPointProperties.codeMemorySize);

            shader.entryPoints.push_back({
                .name = entryPointProperties.name,
                .shader = device->CreateShader({
                    .name = SR_FORMAT("Entry point [{0}] of shader [{1}]", entryPointProperties.name, shader.name),
                    .shaderType = entryPointProperties.type,
                    .code = code
                })
            });
        }
    }

}
