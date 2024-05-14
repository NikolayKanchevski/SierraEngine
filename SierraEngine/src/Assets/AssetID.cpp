//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "AssetID.h"

namespace SierraEngine
{

    AssetID::AssetID(const std::filesystem::path &filePath)
        : hash(std::hash<std::string>{}(filePath.string()))
    {

    }

}