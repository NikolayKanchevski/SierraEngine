//
// Created by Nikolay Kanchevski on 27.01.24.
//

#include "MetalComputePipeline.h"

#include "MetalShader.h"

#include "../DeviceErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalComputePipeline::MetalComputePipeline(const MetalDevice& device, const ComputePipelineCreateInfo& createInfo)
        : ComputePipeline(createInfo)
    {
        SR_THROW_IF(createInfo.computeShader.GetBackendType() != RenderingBackendType::Metal, UnexpectedTypeError(SR_FORMAT("Cannot create compute pipeline [{0}] with compute shader [{1}], as its backend type differs from [RenderingBackendType::Metal]", createInfo.name, createInfo.computeShader.GetName())));
        const MetalShader& metalComputeShader = static_cast<const MetalShader&>(createInfo.computeShader);

        // Allocate pipeline descriptor
        const MTLComputePipelineDescriptor* const computePipelineDescriptor = [[MTLComputePipelineDescriptor alloc] init];
        device.SetResourceName(computePipelineDescriptor, createInfo.name);

        // Mark bindless buffer as immutable
        [[[computePipelineDescriptor buffers] objectAtIndexedSubscript: MetalDevice::BINDLESS_ARGUMENT_BUFFER_INDEX] setMutability: MTLMutabilityImmutable];
        [[[computePipelineDescriptor buffers] objectAtIndexedSubscript: MetalDevice::PUSH_CONSTANT_INDEX] setMutability: MTLMutabilityImmutable];

        // Create pipeline
        NSError* error = nil;
        computePipelineState = [device.GetMetalDevice() newComputePipelineStateWithFunction: metalComputeShader.GetEntryFunction() error: &error];
        SR_THROW_IF(error != nil, UnknownDeviceError(SR_FORMAT("Could not create compute pipeline [{0}]", createInfo.name)));

        [computePipelineDescriptor release];
    }

    /* --- GETTER METHODS --- */

    std::string_view MetalComputePipeline::GetName() const noexcept
    {
        return { computePipelineState.label.UTF8String, computePipelineState.label.length };
    }

    /* --- DESTRUCTOR --- */

    MetalComputePipeline::~MetalComputePipeline() noexcept
    {
        [computePipelineState release];
    }

}