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
        [[nodiscard]] inline uint64 GetHash() const { return hash; }

        /* --- OPERATORS --- */
        AssetID(const AssetID &other) = default;
        AssetID& operator=(const AssetID &other) = default;

        inline bool operator==(const AssetID &other) const { return hash == other.hash; }
        inline bool operator!=(const AssetID &other) const { return hash != other.hash; }

        /* --- DESTRUCTOR --- */
        ~AssetID() = default;

    private:
        uint64 hash = 0;

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
