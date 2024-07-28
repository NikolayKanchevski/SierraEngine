//
// Created by Nikolay Kanchevski on 15.07.24.
//

#pragma once

namespace SierraEngine
{

    struct EditorAssetManagerCreateInfo
    {
        const Sierra::RenderingContext &renderingContext;
        ThreadPool &threadPool;
    };

    class EditorAssetManager final : public AssetManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorAssetManager(const EditorAssetManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(Sierra::CommandBuffer &commandBuffer) override;

        void ImportTexture(std::weak_ptr<TextureImporter> importer, AssetLoadCallback Callback) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsTextureImported(const TextureID ID) const override { return textureMap.find(ID) != textureMap.end(); }
        [[nodiscard]] Texture* GetTexture(const TextureID ID) override { const auto iterator = textureMap.find(ID); return iterator != textureMap.end() ? &iterator->second : nullptr; }
        [[nodiscard]] Texture& GetDefaultTexture(TextureType type) override;

        /* --- DESTRUCTOR --- */
        ~EditorAssetManager() override = default;

    private:
        ThreadPool &threadPool;
        const Sierra::RenderingContext &renderingContext;

        TextureID defaultCheckeredTextureID;
        TextureID defaultBlackTextureID;
        TextureID defaultNormalTextureID;
        std::unordered_map<TextureID, Texture> textureMap;

        struct TextureQueueEntry
        {
            TextureID ID;
            Texture texture;
            std::vector<std::unique_ptr<Sierra::Buffer>> levelBuffers;
            AssetLoadCallback LoadCallback = [](const TextureID) -> void { };
        };

        std::mutex textureQueueMutex;
        std::queue<TextureQueueEntry> textureQueue;

    };

}
