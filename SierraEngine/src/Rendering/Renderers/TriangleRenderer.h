//
// Created by Nikolay Kanchevski on 24.06.24.
//

#pragma once

namespace SierraEngine
{

    struct TriangleRendererCreateInfo
    {
        const Sierra::RenderingContext &renderingContext;
        const Sierra::Image &templateOutputImage;
    };

    class TriangleRenderer final : public Sierra::Renderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TriangleRenderer(const TriangleRendererCreateInfo &createInfo);
        
        /* --- POLLING METHODS --- */
        void Resize(uint32 width, uint32 height) override;
        void Render(Sierra::CommandBuffer &commandBuffer, const Sierra::Image &outputImage) override;

        /* --- DESTRUCTOR --- */
        ~TriangleRenderer() override = default;

    private:
        const Sierra::RenderingContext &renderingContext;

        std::unique_ptr<Sierra::Shader> vertexShader = nullptr;
        std::unique_ptr<Sierra::Shader> fragmentShader = nullptr;

        std::unique_ptr<Sierra::RenderPass> renderPass = nullptr;
        std::unique_ptr<Sierra::GraphicsPipeline> graphicsPipeline = nullptr;

    };

}
