//
// Created by Nikolay Kanchevski on 26.07.23.
//

#pragma once

namespace Sierra::Engine
{

    enum class AssetType
    {
        UNKNOWN = -1,
        TEXTURE = 0,
        CUBEMAP = 1,
        MATERIAL = 2,
        MODEL = 3,
        AUDIO = 4
    };

    using AssetUUID = uint64;
    class AssetID
    {
    public:
        /* --- CONSTRUCTORS --- */
        AssetID(const String &assetFilePath);
        static const AssetID Null;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool IsNull() const { return UUID == 0; }
        [[nodiscard]] inline AssetUUID GetUUID() const { return UUID; }
        [[nodiscard]] inline FilePath& GetFilePath() const { return filePath; }
        [[nodiscard]] inline String GetFileName(const bool includeExtension = true) const { return includeExtension ? filePath.filename().string() : filePath.stem().string(); }

        /* --- OPERATORS --- */
		[[nodiscard]] inline operator bool() const { return !IsNull(); }
		inline bool operator==(const AssetID &other) const { return UUID == other.UUID; }
		inline bool operator!=(const AssetID &other) const { return UUID != other.UUID; }

    private:
        AssetID() = default;

        AssetUUID UUID = 0;
        mutable FilePath filePath = "";
        static inline std::hash<String> hasher;

    };

    inline constexpr const char* AssetType_ToString(const AssetType type)
    {
        switch (type)
        {
            case AssetType::UNKNOWN:           return "Unknown";
            case AssetType::MODEL:             return "Model";
            case AssetType::TEXTURE:           return "Texture";
            case AssetType::CUBEMAP:           return "Cubemap";
            case AssetType::MATERIAL:          return "Material";
            case AssetType::AUDIO:             return "Audio";
        }
    }

    inline constexpr AssetType AssetType_FromString(const char* string)
    {
        if (StringView("Model") == string)          return AssetType::MODEL;
        if (StringView("Texture") == string)        return AssetType::TEXTURE;
        if (StringView("Cubemap") == string)        return AssetType::CUBEMAP;
        if (StringView("Material") == string)       return AssetType::MATERIAL;
        if (StringView("Audio") == string)          return AssetType::AUDIO;
        return AssetType::UNKNOWN;
    }
}

// Add automatic hashing of AssetID, so it can be used within hash maps
namespace std
{
    template<>
    struct hash<Sierra::Engine::AssetID>
    {
        inline uSize operator()(const Sierra::Engine::AssetID &assetID) const
        {
            return assetID.GetUUID();
        }
    };
}
