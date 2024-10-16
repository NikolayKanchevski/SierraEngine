//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#if !defined(__OBJC__)
    namespace Sierra
    {
        using MTLComputePipelineState = void;
    }
#endif

#include "../ComputePipeline.h"
#include "MetalResource.h"

#include "MetalDevice.h"

namespace Sierra
{

    class SIERRA_API MetalComputePipeline final : public ComputePipeline, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalComputePipeline(const MetalDevice& device, const ComputePipelineCreateInfo& createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] std::string_view GetName() const noexcept override;
        [[nodiscard]] id<MTLComputePipelineState> GetComputePipelineState() const noexcept { return computePipelineState; }

        /* --- COPY SEMANTICS --- */
        MetalComputePipeline(const MetalComputePipeline&) = delete;
        MetalComputePipeline& operator=(const MetalComputePipeline&) = delete;

        /* --- MOVE SEMANTICS --- */
        MetalComputePipeline(MetalComputePipeline&&) = delete;
        MetalComputePipeline& operator=(MetalComputePipeline&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MetalComputePipeline() noexcept override;

    private:
        id<MTLComputePipelineState> computePipelineState = nil;

    };

}
