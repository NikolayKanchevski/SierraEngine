//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "AssetID.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API AssetManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AssetLoadCallback = std::function<void(AssetID)>;

        /* --- POLLING METHODS --- */
        virtual void Update(Sierra::CommandBuffer &commandBuffer) = 0;

        /* --- OPERATORS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetManager() = default;

    protected:
        AssetManager() = default;

    };

}
