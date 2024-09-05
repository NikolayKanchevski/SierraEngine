//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "MetalComputePipeline.h"

#include "MetalShader.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalComputePipeline::MetalComputePipeline(const MetalDevice& device, const ComputePipelineCreateInfo& createInfo)
        : ComputePipeline(createInfo)
    {
        SR_ERROR_IF(createInfo.computeShader.GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create compute pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", createInfo.name, createInfo.computeShader.GetName());
        const MetalShader& metalComputeShader = static_cast<const MetalShader&>(createInfo.computeShader);

        // Allocate pipeline descriptor
        MTLComputePipelineDescriptor* const computePipelineDescriptor = [[MTLComputePipelineDescriptor alloc] init];
        device.SetResourceName(computePipelineDescriptor, createInfo.name);

        // Mark bindless buffer as immutable
        [[[computePipelineDescriptor buffers] objectAtIndexedSubscript: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX] setMutability: MTLMutabilityImmutable];
        [[[computePipelineDescriptor buffers] objectAtIndexedSubscript: MetalDevice::PUSH_CONSTANT_INDEX] setMutability: MTLMutabilityImmutable];

        // Create pipeline
        NSError* error = nil;
        computePipelineState = [device.GetMetalDevice() newComputePipelineStateWithFunction: metalComputeShader.GetEntryFunction() error: &error];
        SR_ERROR_IF(error != nil, "[Metal]: Could not create compute pipeline [{0}]! Error: {1}.", createInfo.name, [error.description cStringUsingEncoding: NSASCIIStringEncoding]);

        [computePipelineDescriptor release];
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalComputePipeline::GetName() const
    {
        return { [computePipelineState.label UTF8String], [computePipelineState.label length] };
    }

    /* --- DESTRUCTOR --- */

    MetalComputePipeline::~MetalComputePipeline()
    {
        [computePipelineState release];
    }

}