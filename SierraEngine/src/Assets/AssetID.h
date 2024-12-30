//
// Created by Nikolay Kanchevski on 15.11.24.
//

#pragma once

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using AssetID = Sierra::Handle<uint64>;
    struct TextureID  final : public AssetID { };
    struct MaterialID final : public AssetID { };
    struct ModelID    final : public AssetID { };

    /* --- CONCEPTS --- */
    template<typename T>
    concept AssetIDType = std::is_base_of_v<AssetID, T> && !std::is_same_v<AssetID, T>;

}