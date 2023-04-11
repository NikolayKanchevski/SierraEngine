//
// Created by Nikolay Kanchevski on 1.03.23.
//

#pragma once

#include "VulkanRenderer.h"

#define USE_PIPELINE_RENDER_PASS_IMPLEMENTATION
#include "../Abstractions/Pipelines.h"

namespace Sierra::Core::Rendering::Vulkan::Renderers
{

    class ExperimentalVulkanRenderer : public VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        ExperimentalVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<ExperimentalVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Render() override;

        /* --- DESTRUCTOR --- */
        void Destroy() override;
    private:
        UniquePtr<GraphicsPipeline<>> graphicsPipeline;

    };

}