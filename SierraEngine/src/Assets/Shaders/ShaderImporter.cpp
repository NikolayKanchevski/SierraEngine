//
// Created by Nikolay Kanchevski on 18.11.26.
//

#include "ShaderImporter.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    Sierra::StreamResult ShaderImporter::ImportBlobAndProperties(Sierra::Stream& blob, const std::span<const ShaderEntryPoint> entryPoints, const Sierra::RenderingBackendType backendType, ShaderProperties& outProperties, Sierra::WriteStream& outBuffer) const
    {
        std::vector<ShaderEntryPointProperties> entryPointProperties = { };
        entryPointProperties.reserve(entryPoints.size());

        for (const ShaderEntryPoint& entryPoint : entryPoints)
        {
            ShaderEntryPointProperties& properties = entryPointProperties.emplace_back();
            properties.name = entryPoint.name;
            properties.type = entryPoint.type;
            properties.codeOffset = outBuffer.GetOffset();

            size codeOffset = 0;
            switch (backendType)
            {
                case Sierra::RenderingBackendType::Vulkan:
                {
                    codeOffset = entryPoint.spirvCodeOffset;
                    properties.codeMemorySize = entryPoint.spirvCodeMemorySize;
                    break;
                }
                case Sierra::RenderingBackendType::Metal:
                {
                    #if SR_PLATFORM_iOS
                        codeOffset = entryPoint.iOSMetalCodeOffset;
                        properties.codeMemorySize = entryPoint.iOSMetalCodeMemorySize;
                    #else
                        codeOffset = entryPoint.macOSMetalCodeOffset;
                        properties.codeMemorySize = entryPoint.macOSMetalCodeMemorySize;
                    #endif
                    break;
                }
                default:
                {
                    return Sierra::StreamResult::Unknown;
                }
            }

            if (const Sierra::StreamResult result = blob.SetOffset(sizeof(AssetHeader) + codeOffset); result != Sierra::StreamResult::Success)
                return result;

            std::vector<uint8> code(properties.codeMemorySize);
            if (const Sierra::StreamResult result = blob.Read(code); result != Sierra::StreamResult::Success)
                return result;

            if (const Sierra::StreamResult result = outBuffer.Write(code); result != Sierra::StreamResult::Success)
                return result;
        }

        outProperties =
        {
            .entryPoints = std::move(entryPointProperties),
            .backendType = backendType
        };

        return Sierra::StreamResult::Success;
    }

}
