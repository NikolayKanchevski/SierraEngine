//
// Created by Nikolay Kanchevski on 24.07.24.
//

#pragma once

#include "AssetHeader.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API AssetSerializer
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual AssetSignature GetSignature() const noexcept = 0;
        [[nodiscard]] virtual AssetVersion GetVersion() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        AssetSerializer(const AssetSerializer&) = delete;
        AssetSerializer& operator=(const AssetSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetSerializer() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        AssetSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        void SerializeHeader(Sierra::Stream& stream) const;

        /* --- MOVE SEMANTICS --- */
        AssetSerializer(AssetSerializer&&) noexcept = default;
        AssetSerializer& operator=(AssetSerializer&&) noexcept = default;

    };

}
