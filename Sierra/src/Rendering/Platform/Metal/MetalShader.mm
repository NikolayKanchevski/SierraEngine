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
        #if SR_PLATFORM_macOS
            const std::filesystem::path shaderLibraryFilePath = createInfo.shaderBundlePath / "shader.macos.metallib";
        #elif SR_PLATFORM_iOS && !SR_PLATFORM_EMULATOR
            const std::filesystem::path shaderLibraryFilePath = createInfo.shaderBundlePath / "shader.ios.metallib";
        #elif SR_PLATFORM_iOS && SR_PLATFORM_EMULATOR
            const std::filesystem::path shaderLibraryFilePath = createInfo.shaderBundlePath / "shader.ios-simulator.metallib";
        #endif

        SR_ERROR_IF(!File::FileExists(shaderLibraryFilePath), "[Metal]: Could not load Metal library from shader bundle [{0}]! Verify its presence and try again.", createInfo.shaderBundlePath.string().c_str());


        const std::string filePath = shaderLibraryFilePath.string();
        NSString* const name = [[NSString alloc] initWithBytes: filePath.c_str() length: filePath.size() encoding: NSASCIIStringEncoding];

        // Load library
        NSError* error = nil;
        const id<MTLLibrary> library = [device.GetMetalDevice() newLibraryWithURL: [NSURL fileURLWithPath: name] error: &error];
        [name release];

        SR_ERROR_IF(error != nil, "Could not load Metal shader library [{0} - {1}]! Error: {2}.", GetName(), shaderLibraryFilePath.string().c_str(), error.description.UTF8String);
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
