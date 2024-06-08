//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

namespace SierraEngine
{

    class SIERRA_ENGINE_API AssetID final
    {
    public:
        /* --- CONSTRUCTORS --- */
        AssetID() = default;
        AssetID(const std::filesystem::path &filePath);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint64 GetHash() const { return hash; }

        /* --- OPERATORS --- */
        [[nodiscard]] bool operator==(const AssetID &other) const { return hash == other.hash; }
        [[nodiscard]] bool operator!=(const AssetID &other) const { return hash != other.hash; }

        AssetID(const AssetID &other) = default;
        AssetID& operator=(const AssetID &other) = default;

        /* --- DESTRUCTOR --- */
        ~AssetID() = default;

    private:
        uint64 hash = 0;

    };

}

template<>
struct std::hash<SierraEngine::AssetID>
{
    size_t operator()(const SierraEngine::AssetID &assetID) const { return assetID.GetHash(); }
};
