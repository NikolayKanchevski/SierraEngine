//
// Created by Nikolay Kanchevski on 11.12.24.
//

#pragma once

#include "AssetID.h"

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using AssetFormatSignature = std::array<char, 2>;
    using AssetTypeSignature   = std::array<char, 2>;

    using AssetSignature = std::array<char, 2 + 2>;
    using AssetVersion = Sierra::Version;

    struct AssetHeader
    {
        AssetSignature signature = { };
        AssetVersion version = Sierra::Version({ 1, 0, 0 });
    };

}
