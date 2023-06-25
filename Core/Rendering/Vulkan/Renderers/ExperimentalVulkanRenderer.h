//
// Created by Nikolay Kanchevski on 1.03.23.
//

#pragma once

#include "VulkanRenderer.h"

#include "../Abstractions/GraphicsPipeline.h"


namespace Sierra::Core::Rendering::Vulkan::Renderers
{
    using namespace Vulkan::Abstractions;

    class ExperimentalVulkanRenderer : public VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ExperimentalVulkanRenderer(const VulkanRendererCreateInfo &createInfo);
        static UniquePtr<ExperimentalVulkanRenderer> Create(VulkanRendererCreateInfo createInfo);

        /* --- POLLING METHODS --- */
        void Render() override;

        /* --- DESTRUCTOR --- */
        void Destroy() override;
    private:
        UniquePtr<GraphicsPipeline> graphicsPipeline;

    };

}