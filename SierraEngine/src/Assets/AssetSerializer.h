//
// Created by Nikolay Kanchevski on 24.07.24.
//

#pragma once

#include "AssetHeader.h"

namespace SierraEngine
{

    struct SerializedAsset
    {
        std::vector<uint8> data = { };
        std::vector<uint8> blob = { };
    };

    class SIERRA_ENGINE_API AssetSerializer
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual AssetSignature GetSignature() const noexcept;
        [[nodiscard]] virtual AssetVersion GetVersion() const noexcept = 0;

        [[nodiscard]] virtual AssetFormatSignature GetFormatSignature() const noexcept = 0;
        [[nodiscard]] virtual AssetTypeSignature GetTypeSignature() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        AssetSerializer(const AssetSerializer&) = delete;
        AssetSerializer& operator=(const AssetSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetSerializer() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        AssetSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] Sierra::StreamResult SerializeHeader(Sierra::Stream& stream) const;

        /* --- MOVE SEMANTICS --- */
        AssetSerializer(AssetSerializer&&) noexcept = default;
        AssetSerializer& operator=(AssetSerializer&&) noexcept = default;

    };

}
