//
// Created by Nikolay Kanchevski on 30.10.24.
//

#pragma once

#include "AssetHeader.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API AssetImporter
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual AssetSignature GetSignature() const noexcept = 0;
        [[nodiscard]] virtual AssetVersion GetVersion() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        AssetImporter(const AssetImporter&) = delete;
        AssetImporter& operator=(const AssetImporter&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetImporter() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        AssetImporter() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        AssetImporter(AssetImporter&&) noexcept = default;
        AssetImporter& operator=(AssetImporter&&) noexcept = default;

    };

}