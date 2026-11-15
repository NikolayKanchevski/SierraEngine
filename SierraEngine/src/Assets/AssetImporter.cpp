//
// Created by Nikolay Kanchevski on 25.12.24.
//

#include "AssetImporter.h"

namespace SierraEngine
{

    AssetSignature AssetImporter::GetSignature() const noexcept
    {
        const AssetFormatSignature formatSignature = GetFormatSignature();
        const AssetTypeSignature typeSignature = GetTypeSignature();

        return { formatSignature[0], formatSignature[1], typeSignature[0], typeSignature[1] };
    }

}