//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "MetalComputePipeline.h"

#include "MetalShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalComputePipeline::MetalComputePipeline(const MetalDevice &device, const ComputePipelineCreateInfo &createInfo)
        : ComputePipeline(createInfo), MetalResource(createInfo.name), layout(static_cast<MetalPipelineLayout&>(*createInfo.layout))
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create compute pipeline [{0}] using pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Metal]!");

        SR_ERROR_IF(createInfo.computeShader->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create compute pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.computeShader->GetName());
        const MetalShader &metalComputeShader = static_cast<MetalShader&>(*createInfo.computeShader);

        // Allocate pipeline descriptor
        MTL::ComputePipelineDescriptor* computePipelineDescriptor = MTL::ComputePipelineDescriptor::alloc()->init();
        device.SetResourceName(computePipelineDescriptor, GetName());

        // Create pipeline
        NS::Error* error = nullptr;
        computePipelineState = device.GetMetalDevice()->newComputePipelineState(metalComputeShader.GetEntryFunction(), &error);
        SR_ERROR_IF(error != nullptr, "[Metal]: Could not create compute pipeline [{0}]! Error: {1}.", GetName(), error->description()->cString(NS::ASCIIStringEncoding));

        computePipelineDescriptor->release();
    }

    /* --- DESTRUCTOR --- */

    MetalComputePipeline::~MetalComputePipeline()
    {
        computePipelineState->release();
    }

}