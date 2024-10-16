//
// Created by Nikolay Kanchevski on 9.01.24.
//

#pragma once

#include "RenderingResource.h"

namespace Sierra
{

    enum class PipelineBindingType : uint8
    {
        Undefined,
        UniformBuffer,
        StorageBuffer,
        Image,
        Texture,
        InputAttachment
    };

    struct PipelineBinding
    {
        PipelineBindingType type = PipelineBindingType::Undefined;
        uint32 arraySize = 1;
    };

    struct PipelineLayoutCreateInfo
    {
        const std::string &name = "Pipeline Layout";
        const std::initializer_list<PipelineBinding> &bindings = { };
        uint16 pushConstantSize = 0;
    };

    class SIERRA_API PipelineLayout : public virtual RenderingResource
    {
    public:
        /* --- OPERATORS --- */
        PipelineLayout(const PipelineLayout&) = delete;
        PipelineLayout &operator=(const PipelineLayout&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~PipelineLayout() = default;

    protected:
        explicit PipelineLayout(const PipelineLayoutCreateInfo &createInfo);

    };

}
