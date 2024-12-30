//
// Created by Nikolay Kanchevski on 25.12.24.
//

#include "AssetImporter.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    AssetHeader AssetImporter::ImportHeader(Sierra::Stream& stream) const
    {
        return stream.Read<AssetHeader>();
    }

}