//
// Created by Nikolay Kanchevski on 9.10.25.
//

#pragma once
#include "../Core/Project.h"

namespace SierraEngine
{
    class EditorAssetManager final : public AssetManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorAssetManager(const AssetManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update(Sierra::CommandBuffer& commandBuffer) override;

        void LoadProjectAssets(const Sierra::FileManager& fileManager, const Project& project);

        void ImportTexture(const ImportedTexture& importedTexture) override;
        void ImportMaterial(const ImportedMaterial& importedMaterial) override;

        /* --- GETTER METHODS --- */
        // [[nodiscard]] const TextureAsset& GetDefaultTexture(TextureType textureType) const noexcept override { return *reinterpret_cast<TextureAsset*>(intptr_t(0)); }
        // [[nodiscard]] const MaterialAsset& GetDefaultMaterial() const noexcept override { return *reinterpret_cast<MaterialAsset*>(intptr_t(0)); }
        //
        // [[nodiscard]] const TextureAsset* GetTexture(TextureID ID) const noexcept override { return nullptr; }
        // [[nodiscard]] const MaterialAsset* GetMaterial(MaterialID ID) const noexcept override { return nullptr; }

        /* --- COPY SEMANTICS --- */
        EditorAssetManager(const EditorAssetManager &) = delete;
        EditorAssetManager& operator=(const EditorAssetManager &) = delete;

        /* --- MOVE SEMANTICS --- */
        EditorAssetManager(EditorAssetManager &&) noexcept = default;
        EditorAssetManager& operator=(EditorAssetManager &&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~EditorAssetManager() noexcept override = default;

    private:

    };
}