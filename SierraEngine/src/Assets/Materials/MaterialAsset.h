//
// Created by Nikolay Kanchevski on 19.11.24.
//

#pragma once

#include "../Asset.h"
#include "../AssetID.h"
#include "../../Rendering/Material.h"

namespace SierraEngine
{

    enum class MaterialAlphaMode : bool
    {
        Opaque,
        Transparent
    };

    enum class MaterialCullMode : bool
    {
        SingleSided,
        DoubleSided
    };

    struct MaterialCreateInfo
    {
        MaterialAlphaMode alphaMode = MaterialAlphaMode::Opaque;
        MaterialCullMode cullMode = MaterialCullMode::SingleSided;
    };

    class SIERRA_ENGINE_API MaterialAsset final : public Asset
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MaterialAsset(const MaterialCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] MaterialAlphaMode GetAlphaMode() const noexcept { return alphaMode; }
        [[nodiscard]] MaterialCullMode GetCullMode() const noexcept { return cullMode; }

        [[nodiscard]] AssetType GetType() const noexcept override { return AssetType::Material; }

        /* --- COPY SEMANTICS --- */
        MaterialAsset(const MaterialAsset&) = delete;
        MaterialAsset& operator=(const MaterialAsset&) = delete;

        /* --- MOVE SEMANTICS --- */
        MaterialAsset(MaterialAsset&&) noexcept = default;
        MaterialAsset& operator=(MaterialAsset&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MaterialAsset() noexcept override = default;

    private:
        MaterialAlphaMode alphaMode = MaterialAlphaMode::Opaque;
        MaterialCullMode cullMode = MaterialCullMode::SingleSided;

    };

}