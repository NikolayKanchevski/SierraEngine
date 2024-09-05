//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "MetalShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalShader::MetalShader(const MetalDevice& device, const ShaderCreateInfo& createInfo)
        : Shader(createInfo)
    {
        const ShaderFileHeader& fileHeader = *reinterpret_cast<const ShaderFileHeader*>(createInfo.memory.data());
        #if SR_PLATFORM_macOS
            const void* shaderData = createInfo.memory.data() + sizeof(ShaderFileHeader) + fileHeader.GetMacOSMetalLibOffset();
            const size shaderDataMemorySize = fileHeader.macOSMetalLibMemorySize;
        #elif SR_PLATFORM_iOS && !SR_PLATFORM_EMULATOR
            const void* shaderData = createInfo.memory.data() + sizeof(ShaderFileHeader) + fileHeader.GetIOSMetalLibOffset();
            const size shaderDataMemorySize = fileHeader.iOSMetalLibMemorySize;
        #elif SR_PLATFORM_iOS && SR_PLATFORM_EMULATOR
            const void* shaderData = createInfo.memory.data() + sizeof(ShaderFileHeader) + fileHeader.GetIOSSimulatorMetalLibOffset();
            const size shaderDataMemorySize = fileHeader.iOSSimulatorMetalLibMemorySize;
        #endif

        // Load library
        NSError* error = nil;
        const id<MTLLibrary> library = [device.GetMetalDevice() newLibraryWithData: dispatch_data_create(shaderData, shaderDataMemorySize, nullptr, nullptr) error: &error];

        SR_ERROR_IF(error != nil, "[Metal]: Could not load Metal shader library [{0}]! Error: {1}.", createInfo.name, error.description.UTF8String);
        device.SetResourceName(library, createInfo.name);

        // Load entry point
        entryFunction = [library newFunctionWithName: @"main0"];
        [library release];
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalShader::GetName() const
    {
        return { [entryFunction.label UTF8String], [entryFunction.label length] };
    }

    /* --- DESTRUCTOR --- */

    MetalShader::~MetalShader()
    {
        [entryFunction release];
    }

}
