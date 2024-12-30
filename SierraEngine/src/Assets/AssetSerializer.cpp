//
// Created by Nikolay Kanchevski on 25.12.24.
//

#include "AssetSerializer.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void AssetSerializer::SerializeHeader(Sierra::Stream& stream) const
    {
        const AssetHeader header
        {
            .signature = GetSignature(),
            .version = GetVersion()
        };
        stream.Write(header);
    }

}