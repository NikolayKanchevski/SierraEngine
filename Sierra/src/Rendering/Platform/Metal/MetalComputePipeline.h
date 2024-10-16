//
// Created by Nikolay Kanchevski on 27.01.24.
//

#pragma once

#include "../../ComputePipeline.h"
#include "MetalResource.h"

#include "MetalDevice.h"
#include "MetalPipelineLayout.h"

namespace Sierra
{

    class SIERRA_API MetalComputePipeline : public ComputePipeline, public MetalResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        MetalComputePipeline(const MetalDevice &device, const ComputePipelineCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline id<MTLComputePipelineState> GetComputePipelineState() const { return computePipelineState; }
        [[nodiscard]] inline const MetalPipelineLayout& GetLayout() const { return layout; }

        /* --- DESTRUCTOR --- */
        ~MetalComputePipeline() override;

    private:
        const MetalPipelineLayout &layout;
        id<MTLComputePipelineState> computePipelineState = nil;

    };

}
