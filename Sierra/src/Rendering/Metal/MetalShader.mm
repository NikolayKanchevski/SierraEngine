//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "MetalShader.h"

#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalShader::MetalShader(const MetalDevice& device, const ShaderCreateInfo& createInfo)
        : MetalResource(createInfo.name), Shader(createInfo)
    {
        // Compile library from source (the version must match the one the source was translated to)
        NSString* const source = [[NSString alloc] initWithBytes: createInfo.code.data() length: createInfo.code.size() encoding: NSUTF8StringEncoding];
        MTLCompileOptions* const compileOptions = [[MTLCompileOptions alloc] init];
        [compileOptions setLanguageVersion: MTLLanguageVersion3_0];

        NSError* error = nil;
        const id<MTLLibrary> library = [device.GetMetalDevice() newLibraryWithSource: source options: compileOptions error: &error];
        [compileOptions release];
        [source release];
        SR_THROW_IF(library == nil, UnknownDeviceError(SR_FORMAT("Could not create shader [{0}], as its Metal source could not be compiled: {1}", createInfo.name, error != nil ? error.localizedDescription.UTF8String : "Unknown error")));

        // Load entry point
        entryFunction = [library newFunctionWithName: @"main0"];
        [library release];
        SR_THROW_IF(entryFunction == nil, UnknownDeviceError(SR_FORMAT("Could not create shader [{0}], as its Metal source has no entry point [main0]", createInfo.name)));
        device.SetResourceName(entryFunction, SR_FORMAT("Entry function of shader [{0}]", createInfo.name));
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalShader::GetName() const noexcept
    {
        return { [entryFunction.label UTF8String], [entryFunction.label length] };
    }

    /* --- DESTRUCTOR --- */

    MetalShader::~MetalShader() noexcept
    {
        [entryFunction release];
    }

}
