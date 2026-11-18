//
// Created by Nikolay Kanchevski on 25.12.24.
//

#include "AssetSerializer.h"

#include "AssetHeader.h"

namespace SierraEngine
{

    AssetSignature AssetSerializer::GetSignature() const noexcept
    {
        const AssetFormatSignature formatSignature = GetFormatSignature();
        const AssetTypeSignature typeSignature = GetTypeSignature();

        return { formatSignature[0], formatSignature[1], typeSignature[0], typeSignature[1] };
    }

    /* --- POLLING METHODS --- */

    Sierra::StreamResult AssetSerializer::SerializeHeader(Sierra::Stream& stream) const
    {
        const AssetHeader header
        {
            .signature = GetSignature(),
            .version = GetVersion()
        };

        return stream.WriteAs(header);
    }

}