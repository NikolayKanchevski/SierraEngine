//
// Created by Nikolay Kanchevski on 11.12.24.
//

#pragma once

#include "AssetID.h"

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using AssetSignature = std::array<char, 4>;
    using AssetVersion = Sierra::Version;

    struct AssetHeader
    {
        AssetSignature signature = { };
        AssetVersion version = Sierra::Version({ 1, 0, 0 });
    };

}
