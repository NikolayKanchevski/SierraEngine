//
// Created by Nikolay Kanchevski on 26.01.24.
//

#pragma once

#include "RenderingResource.h"

#include "Shader.h"

namespace Sierra
{

    struct ComputePipelineCreateInfo
    {
        std::string_view name = "Compute Pipeline";
        const Shader& computeShader;
        uint16 pushConstantSize = 0;
    };

    class SIERRA_API ComputePipeline : public virtual RenderingResource
    {
    public:
        /* --- COPY SEMANTICS --- */
        ComputePipeline(const ComputePipeline&) = delete;
        ComputePipeline& operator=(const ComputePipeline&) = delete;

        /* --- MOVE SEMANTICS --- */
        ComputePipeline(ComputePipeline&&) = delete;
        ComputePipeline& operator=(ComputePipeline&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ComputePipeline() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit ComputePipeline(const ComputePipelineCreateInfo& createInfo);

    };

}
