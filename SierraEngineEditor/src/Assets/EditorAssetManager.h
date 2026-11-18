//
// Created by Nikolay Kanchevski on 9.10.25.
//

#pragma once

#include "../Core/ApplicationFrame.h"

namespace SierraEngine
{
    class EditorAssetManager final : public AssetManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorAssetManager(const AssetManagerCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Update(const DrawFrame& drawFrame) override;

        void LoadProjectAssets(const ApplicationFrame& applicationFrame);

        void ImportTexture(const ImportedTexture& importedTexture) override;
        void ImportMaterial(const ImportedMaterial& importedMaterial) override;

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