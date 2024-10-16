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
        Sierra::Hash64 hash = 0;
        std::span<const std::string_view> tags = { };
    };

    class AssetSerializer
    {
    public:
        /* --- COPY SEMANTICS --- */
        AssetSerializer(const AssetSerializer&) = delete;
        AssetSerializer& operator=(const AssetSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetSerializer() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        AssetSerializer() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        AssetSerializer(AssetSerializer&&) noexcept = default;
        AssetSerializer& operator=(AssetSerializer&&) noexcept = default;

    };

}
