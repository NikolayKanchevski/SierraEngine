//
// Created by Nikolay Kanchevski on 11.11.24.
//

#include "SceneRenderer.h"

namespace SierraEngine
{
    namespace
    {

        constexpr Sierra::ImageFormat PREFERRED_COLOR_IMAGE_FORMAT = Sierra::ImageFormat::R8G8B8_UNorm;
        constexpr Sierra::ImageUsage REQUIRED_COLOR_IMAGE_USAGE = Sierra::ImageUsage::ColorAttachment | Sierra::ImageUsage::Sample;

        constexpr Sierra::ImageFormat PREFERRED_DEPTH_IMAGE_FORMAT = Sierra::ImageFormat::D16_UNorm;
        constexpr Sierra::ImageUsage REQUIRED_DEPTH_IMAGE_USAGE = Sierra::ImageUsage::DepthAttachment | Sierra::ImageUsage::Sample;
    }

    /* --- CONSTRUCTORS --- */

    SceneRenderer::SceneRenderer(const SceneRendererCreateInfo& createInfo)
        : renderingContext(&createInfo.renderingContext)
    {
        const Sierra::Device& device = renderingContext->GetDevice();

        const std::optional<Sierra::ImageFormat> suitableColorFormat = device.GetSupportedImageFormat(PREFERRED_COLOR_IMAGE_FORMAT, REQUIRED_COLOR_IMAGE_USAGE);
        APP_THROW_IF(!suitableColorFormat.has_value(), Sierra::UnsupportedFeatureError(SR_FORMAT("Cannot create scene renderer, as device [{0}] does not support any suitable color image format", device.GetName())));
        colorFormat = *suitableColorFormat;

        const std::optional<Sierra::ImageFormat> suitableDepthFormat = device.GetSupportedImageFormat(PREFERRED_DEPTH_IMAGE_FORMAT, REQUIRED_DEPTH_IMAGE_USAGE);
        APP_THROW_IF(!suitableDepthFormat.has_value(), Sierra::UnsupportedFeatureError(SR_FORMAT("Cannot create scene renderer, as device [{0}] does not support any suitable depth image format", device.GetName())));
        depthFormat = *suitableDepthFormat;

        renderPass = device.CreateRenderPass({
            .name = "Render Pass of Scene Renderer",
            .attachments = {{
                Sierra::RenderPassAttachment {
                    .type = Sierra::RenderPassAttachmentType::Color,
                    .format = colorFormat,
                    .loadOperation = Sierra::RenderPassAttachmentLoadOperation::Clear,
                    .storeOperation = Sierra::RenderPassAttachmentStoreOperation::Store
                },
                Sierra::RenderPassAttachment {
                    .type = Sierra::RenderPassAttachmentType::Depth,
                    .format = depthFormat,
                    .loadOperation = Sierra::RenderPassAttachmentLoadOperation::Clear,
                    .storeOperation = Sierra::RenderPassAttachmentStoreOperation::Store
                }
            }},
            .subpassDescriptions = {{
                Sierra::SubpassDescription {
                    .renderTargets = {{ 0, 1 }}
                }
            }}
        });

        graphicsPipeline = CreateGraphicsPipeline(createInfo.vertexShader, createInfo.fragmentShader);

        // Placed in front of where viewport cameras start out (at the origin, looking down +Z, which leaves +X on the left of the screen)
        constexpr std::array<TriangleVertex, 3> TRIANGLE_VERTICES
        {
            TriangleVertex { .position = {  0.0f,  0.5f, 2.0f }, .color = { 235, 64, 52, 255 } }, // Top, red
            TriangleVertex { .position = {  0.5f, -0.5f, 2.0f }, .color = { 52, 199, 89, 255 } }, // Bottom left, green
            TriangleVertex { .position = { -0.5f, -0.5f, 2.0f }, .color = { 10, 132, 255, 255 } } // Bottom right, blue
        };

        // It never changes, so it is written once, straight from RAM
        triangleVertexBuffer = device.CreateBuffer({
            .name = "Triangle Vertex Buffer of Scene Renderer",
            .memorySize = sizeof(TRIANGLE_VERTICES),
            .usage = Sierra::BufferUsage::Vertex,
            .memoryLocation = Sierra::BufferMemoryLocation::RAM
        });
        triangleVertexBuffer->Write(0, { reinterpret_cast<const uint8*>(TRIANGLE_VERTICES.data()), sizeof(TRIANGLE_VERTICES) });
    }

    /* --- POLLING METHODS --- */

    void SceneRenderer::SetShaders(const Sierra::Shader& vertexShader, const Sierra::Shader& fragmentShader)
    {
        // NOTE: Frames in flight may still be using the current pipeline, so it is only queued for destruction
        renderingContext->GetDestructionScheduler().QueueResource(std::move(graphicsPipeline));
        graphicsPipeline = CreateGraphicsPipeline(vertexShader, fragmentShader);
    }

    RenderTargetID SceneRenderer::CreateRenderTarget()
    {
        return renderTargets.AddItem();
    }

    bool SceneRenderer::DestroyRenderTarget(const RenderTargetID ID)
    {
        RenderTarget* renderTarget = renderTargets.GetItem(ID);
        if (renderTarget == nullptr) return false;

        DestroyRenderTarget(*renderTarget);
        return renderTargets.RemoveItem(ID);
    }

    const Sierra::Image& SceneRenderer::Render(const DrawFrame& drawFrame, const SceneRenderInfo& renderInfo)
    {
        RenderTarget* renderTarget = renderTargets.GetItem(renderInfo.renderTarget);
        APP_THROW_IF(renderTarget == nullptr, Sierra::InvalidValueError(SR_FORMAT("Cannot render scene, as specified render target handle [{0}] is invalid", renderInfo.renderTarget.GetValue())));

        if (!renderTarget->IsValid())
        {
            CreateRenderTarget(*renderTarget, renderInfo.width, renderInfo.height);
        }
        else if (renderInfo.width != renderTarget->framebuffer->GetWidth() || renderInfo.height != renderTarget->framebuffer->GetHeight())
        {
            DestroyRenderTarget(*renderTarget);
            CreateRenderTarget(*renderTarget, renderInfo.width, renderInfo.height);
        }

        drawFrame.commandBuffer.SynchronizeImageUsage(*renderTarget->colorImage, { .nextUsage = Sierra::ImageCommandUsage::ColorWrite });
        drawFrame.commandBuffer.SynchronizeImageUsage(*renderTarget->depthImage, { .nextUsage = Sierra::ImageCommandUsage::DepthWrite });

        drawFrame.commandBuffer.BeginRenderPass(*renderPass, *renderTarget->framebuffer);
        drawFrame.commandBuffer.BeginGraphicsPipeline(*graphicsPipeline);
        drawFrame.commandBuffer.BindVertexBuffer(*triangleVertexBuffer, 0);

        const PushConstant pushConstant
        {
            .view = renderInfo.camera.GetViewMatrix(renderInfo.eye),
            .projection = renderInfo.camera.GetProjectionMatrix(static_cast<float32>(renderInfo.width), static_cast<float32>(renderInfo.height))
        };
        drawFrame.commandBuffer.PushConstantsAs(pushConstant);

        drawFrame.commandBuffer.Draw(3, 0);

        drawFrame.commandBuffer.EndGraphicsPipeline();
        drawFrame.commandBuffer.EndRenderPass();

        drawFrame.commandBuffer.SynchronizeImageUsage(*renderTarget->colorImage, { .previousUsage = Sierra::ImageCommandUsage::ColorWrite, .nextUsage = Sierra::ImageCommandUsage::GraphicsRead });
        return *renderTarget->colorImage;
    }

    /* --- GETTER METHODS --- */

    const Sierra::Image* SceneRenderer::GetRenderTargetImage(const RenderTargetID ID)
    {
        if (const RenderTarget* renderTarget = renderTargets.GetItem(ID))
        {
            return renderTarget->colorImage.get();
        }

        return nullptr;
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Sierra::GraphicsPipeline> SceneRenderer::CreateGraphicsPipeline(const Sierra::Shader& vertexShader, const Sierra::Shader& fragmentShader) const
    {
        return renderingContext->GetDevice().CreateGraphicsPipeline({
            .name = "Graphics Pipeline of Scene Renderer",
            .vertexInputs = {{ Sierra::VertexInput::Position_3D, Sierra::VertexInput::Color }},
            .vertexShader = vertexShader,
            .fragmentShader = &fragmentShader,
            .pushConstantSize = sizeof(PushConstant),
            .templateRenderPass = *renderPass,
            .depthMode = Sierra::DepthMode::WriteDepth,
            .cullMode = Sierra::CullMode::None // So that the triangle shows from either side
        });
    }

    void SceneRenderer::CreateRenderTarget(RenderTarget& renderTarget, const uint32 width, const uint32 height) const
    {
        const Sierra::Device& device = renderingContext->GetDevice();
        
        renderTarget.colorImage = device.CreateImage({
            .name = SR_FORMAT("Color Image of Render Target"),
            .width = width,
            .height = height,
            .format = colorFormat,
            .usage = REQUIRED_COLOR_IMAGE_USAGE
        });

        renderTarget.depthImage = device.CreateImage({
            .name = SR_FORMAT("Depth Image of Render Target"),
            .width = width,
            .height = height,
            .format = depthFormat,
            .usage = REQUIRED_DEPTH_IMAGE_USAGE
        });

        renderTarget.framebuffer = device.CreateFramebuffer({
            .name = SR_FORMAT("Framebuffer of Render Target"),
            .width = width,
            .height = height,
            .templateRenderPass = *renderPass,
            .attachments = {{
                Sierra::FramebufferAttachment {
                    .image = *renderTarget.colorImage
                },
                Sierra::FramebufferAttachment {
                    .image = *renderTarget.depthImage
                }
            }}
        });
    }

    void SceneRenderer::DestroyRenderTarget(RenderTarget& renderTarget) const
    {
        Sierra::DestructionScheduler& destructionScheduler = renderingContext->GetDestructionScheduler();
        destructionScheduler.QueueResources(std::move(renderTarget.colorImage), std::move(renderTarget.depthImage), std::move(renderTarget.framebuffer));
    }

}
