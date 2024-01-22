//
// Created by Nikolay Kanchevski on 28.12.23.
//

#include "MetalShader.h"

#include "../../../Engine/File.h"

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

        // Load library
        NS::Error* error = nullptr;
        library = device.GetMetalDevice()->newLibrary(NS::String::string(shaderLibraryFilePath.string().c_str(), NS::ASCIIStringEncoding), &error);
        SR_ERROR_IF(error != nullptr, "Could not load Metal shader library [{0} - {1}]! Error: {2}.", GetName(), shaderLibraryFilePath.string().c_str(), error->localizedDescription()->cString(NS::ASCIIStringEncoding));
        MTL_SET_OBJECT_NAME(library, GetName());

        // Load entry point
        entryFunction = library->newFunction(NS::String::string("main0", NS::ASCIIStringEncoding));
    }

    /* --- DESTRUCTOR --- */

    MetalShader::~MetalShader()
    {
        entryFunction->release();
        library->release();
    }

}
