//
// Created by Nikolay Kanchevski on 23.04.24.
//

#pragma once

namespace SierraEngine
{

    struct SerializedAssetHeader
    {
        const std::array<char, 4> SIGNATURE = { 'S', 'R', 'S', 'A' };
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });

        [[nodiscard]] inline bool IsValid() const { return SIGNATURE == std::array<char, 4>{ 'S', 'R', 'S', 'A' }; }
        [[nodiscard]] inline bool IsOutdated(const Sierra::Version targetVersion) const { return targetVersion > version; }
    };

}