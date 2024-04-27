//
// Created by Nikolay Kanchevski on 23.04.24.
//

#pragma once

#include <entt/entt.hpp>

#include "SerializedAsset.h"

namespace SierraEngine
{

    struct SerializedComponent
    {
        std::array<char, 4> signature = { 'S', 'I', 'G', 'N' };
    };

    struct SerializedEntity
    {
        entt::entity entity = entt::null;
        uint32 componentCount = 0;
    };

    struct SerializedSceneIndex
    {
        uint32 entityCount = 0;
    };

    struct SerializedScene
    {
        SerializedAssetHeader header = { };
        SerializedSceneIndex index = { };
    };

}