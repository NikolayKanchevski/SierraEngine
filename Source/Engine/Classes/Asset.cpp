//
// Created by Nikolay Kanchevski on 7.08.23.
//

#include "Asset.h"

namespace Sierra::Engine
{

    const AssetID AssetID::Null = AssetID();

    /* --- CONSTRUCTORS --- */

    AssetID::AssetID(const String &assetFilePath)
        : filePath(assetFilePath), UUID(hasher(assetFilePath))
    {

    }

}