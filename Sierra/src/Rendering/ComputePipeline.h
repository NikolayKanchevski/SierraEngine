//
// Created by Nikolay Kanchevski on 26.01.24.
//

#pragma once

#include "RenderingResource.h"

#include "Shader.h"
#include "PipelineLayout.h"

namespace Sierra
{

    struct ComputePipelineCreateInfo
    {
        const std::string &name = "Compute Pipeline";
        const std::unique_ptr<Shader> &computeShader;
        const std::unique_ptr<PipelineLayout> &layout;
    };

    class SIERRA_API ComputePipeline : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        ComputePipeline(const ComputePipeline&) = delete;
        ComputePipeline &operator=(const ComputePipeline&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ComputePipeline() = default;

    protected:
        explicit ComputePipeline(const ComputePipelineCreateInfo &createInfo);

    };

}
