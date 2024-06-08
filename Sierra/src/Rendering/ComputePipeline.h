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
        const Shader &computeShader;
        uint16 pushConstantSize = 0;
    };

    class SIERRA_API ComputePipeline : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        ComputePipeline(const ComputePipeline&) = delete;
        ComputePipeline& operator=(const ComputePipeline&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ComputePipeline() = default;

    protected:
        explicit ComputePipeline(const ComputePipelineCreateInfo &createInfo);

    };

}
