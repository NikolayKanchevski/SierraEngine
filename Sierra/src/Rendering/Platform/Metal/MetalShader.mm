//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "MetalShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalShader::MetalShader(const MetalDevice &device, const ShaderCreateInfo &createInfo)
        : Shader(createInfo), MetalResource(createInfo.name)
    {
        const ShaderFileHeader &fileHeader = *reinterpret_cast<const ShaderFileHeader*>(createInfo.data.data());
        #if SR_PLATFORM_macOS
            const void* shaderData = createInfo.data.data() + sizeof(ShaderFileHeader) + fileHeader.GetMacOSMetalLibOffset();
            const uint64 shaderDataMemorySize = fileHeader.macOSMetalLibMemorySize;
        #elif SR_PLATFORM_iOS && !SR_PLATFORM_EMULATOR
            const void* shaderData = createInfo.data.data() + sizeof(ShaderFileHeader) + fileHeader.GetIOSMetalLibOffset();
            const uint64 shaderDataMemorySize = fileHeader.iOSMetalLibMemorySize;
        #elif SR_PLATFORM_iOS && SR_PLATFORM_EMULATOR
            const void* shaderData = createInfo.data.data() + sizeof(ShaderFileHeader) + fileHeader.GetIOSSimulatorMetalLibOffset();
            const uint64 shaderDataMemorySize = fileHeader.iOSSimulatorMetalLibMemorySize;
        #endif

        // Load library
        NSError* error = nil;
        const id<MTLLibrary> library = [device.GetMetalDevice() newLibraryWithData: dispatch_data_create(shaderData, shaderDataMemorySize, nullptr, nullptr) error: &error];

        SR_ERROR_IF(error != nil, "[Metal]: Could not load Metal shader library [{0}]! Error: {1}.", GetName(), error.description.UTF8String);
        device.SetResourceName(library, GetName());

        // Load entry point
        entryFunction = [library newFunctionWithName: @"main0"];
        [library release];
    }

    /* --- DESTRUCTOR --- */

    MetalShader::~MetalShader()
    {
        [entryFunction release];
    }

}
