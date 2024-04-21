//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

namespace SierraEngine
{

    class AssetID final
    {
    public:
        /* --- CONSTRUCTORS --- */
        AssetID() = default;
        inline AssetID(const std::filesystem::path &filePath) : hash(std::hash<std::string>{}(filePath.string())) { };

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline size GetHash() const { return hash; }

        /* --- OPERATORS --- */
        AssetID(const AssetID &other) = default;
        AssetID& operator=(const AssetID &other) = default;

        inline bool operator==(const AssetID &other) const { return hash == other.hash; }
        inline bool operator!=(const AssetID &other) const { return hash != other.hash; }

        /* --- DESTRUCTOR --- */
        ~AssetID() = default;

    private:
        size hash = 0;

    };

    class Asset
    {
    public:
        /* --- OPERATORS --- */
        Asset(const Asset&) = delete;
        Asset& operator=(const Asset&) = delete;

        /* --- MOVE SEMANTICS --- */
        Asset(Asset&& other) = default;
        Asset& operator=(Asset&& other) = default;

        /* --- DESTRUCTOR --- */
        ~Asset() = default;

    protected:
        Asset() = default;

    };

    struct SerializedAssetHeader
    {
        char SIGNATURE[4] = { 'S', 'R', 'S', 'A' };
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
    };

}

namespace std
{
    template<>
    struct hash<SierraEngine::AssetID>
    {
        inline size_t operator()(const SierraEngine::AssetID &assetID) const { return assetID.GetHash(); }
    };
}