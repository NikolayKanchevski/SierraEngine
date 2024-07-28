//
// Created by Nikolay Kanchevski on 24.07.24.
//

#pragma once

namespace SierraEngine
{

    struct AssetMetadata
    {
        std::string_view name = "Asset";
        std::string_view author = "Unknown";
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
        std::span<const std::string_view> tags = { };
    };

    class AssetSerializer
    {
    public:
        /* --- OPERATORS --- */
        AssetSerializer(const AssetSerializer&) = delete;
        AssetSerializer& operator=(const AssetSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetSerializer() = default;

    protected:
        AssetSerializer() = default;

    };

}
