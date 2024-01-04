//
// Created by Nikolay Kanchevski on 1.01.24.
//

#include "MetalGraphicsPipeline.h"

#include "MetalShader.h"
#include "MetalRenderPass.h"
#include "MetalImage.h"
#include "MetalCommandBuffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalGraphicsPipeline::MetalGraphicsPipeline(const Sierra::MetalDevice &device, const Sierra::GraphicsPipelineCreateInfo &createInfo)
        : GraphicsPipeline(createInfo), MetalResource(createInfo.name), cullMode(CullModeToCullMode(createInfo.cullMode)), triangleFillMode(ShadeModeToTriangleFillMode(createInfo.shadeMode)), frontFacingWinding(FrontFaceModeToWinding(createInfo.frontFaceMode))
    {
        SR_ERROR_IF(createInfo.vertexShader->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.vertexShader->GetName());
        const MetalShader &metalVertexShader = static_cast<MetalShader&>(*createInfo.vertexShader);

        SR_ERROR_IF(createInfo.renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.renderPass->GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*createInfo.renderPass);

        // Configure pipeline descriptor's shaders
        MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        MTL_SET_RESOURCE_NAME(renderPipelineDescriptor, GetName().c_str());
        renderPipelineDescriptor->setVertexFunction(metalVertexShader.GetEntryFunction());
        if (createInfo.fragmentShader.has_value())
        {
            SR_ERROR_IF(createInfo.fragmentShader.value().get()->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.fragmentShader.value().get()->GetName());
            const MetalShader &metalFragmentShader = static_cast<MetalShader&>(*createInfo.fragmentShader.value().get());
            renderPipelineDescriptor->setFragmentFunction(metalFragmentShader.GetEntryFunction());
        }

        // Set sample count
        renderPipelineDescriptor->setSampleCount(MetalImage::ImageSamplingToUInteger(createInfo.sampling));

        // Configure attachments
        uint32 colorAttachmentIndex = 0;
        for (const auto &attachment : metalRenderPass.GetAttachmentTable())
        {
            if (attachment.attachmentType == AttachmentType::Color)
            {
                renderPipelineDescriptor->colorAttachments()->object(colorAttachmentIndex)->setPixelFormat(attachment.attachmentDescriptor->texture()->pixelFormat());
                colorAttachmentIndex++;
            }
            else
            {
                renderPipelineDescriptor->setDepthAttachmentPixelFormat(attachment.attachmentDescriptor->texture()->pixelFormat());
            }

        }

        // Set up vertex attributes
        uint32 nextVertexInputOffset = 0;
        MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        for (uint32 i = 0; i < createInfo.vertexInputs.size(); i++)
        {
            vertexDescriptor->attributes()->object(i)->setBufferIndex(0);
            vertexDescriptor->attributes()->object(i)->setOffset(nextVertexInputOffset);
            switch (*(createInfo.vertexInputs.begin() + i))
            {
                case VertexInput::Float:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat);
                    nextVertexInputOffset += sizeof(float32) * 1;
                    break;
                }
                case VertexInput::Float2:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat2);
                    nextVertexInputOffset += sizeof(float32) * 2;
                    break;
                }
                case VertexInput::Float3:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat3);
                    nextVertexInputOffset += sizeof(float32) * 3;
                    break;
                }
                case VertexInput::Float4:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat4);
                    nextVertexInputOffset += sizeof(float32) * 4;
                    break;
                }
            }
        }
        vertexDescriptor->layouts()->object(0)->setStride(nextVertexInputOffset);

        // Create pipeline
        NS::Error* error = nullptr;
        renderPipelineState = device.GetMetalDevice()->newRenderPipelineState(renderPipelineDescriptor, &error);
        SR_ERROR_IF(error != nullptr, "Could not create Metal graphics pipeline [{0}]! Error: {1}.", GetName(), error->localizedDescription()->cString(NS::ASCIIStringEncoding));

        renderPipelineDescriptor->release();
        vertexDescriptor->release();
    }

    /* --- POLLING METHODS --- */

    void MetalGraphicsPipeline::Begin(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot begin graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        // Begin pipeline
        MTL::RenderCommandEncoder* renderCommandEncoder = metalCommandBuffer.GetCurrentRenderEncoder();
        renderCommandEncoder->setCullMode(cullMode);
        renderCommandEncoder->setTriangleFillMode(triangleFillMode);
        renderCommandEncoder->setFrontFacingWinding(frontFacingWinding);
        renderCommandEncoder->setRenderPipelineState(renderPipelineState);
    }

    void MetalGraphicsPipeline::End(std::unique_ptr<CommandBuffer> &commandBuffer) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot end graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
    }

    void MetalGraphicsPipeline::Draw(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 vertexCount) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot draw graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        // Draw vertices
        metalCommandBuffer.GetCurrentRenderEncoder()->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), vertexCount);
    }
    
    /* --- DESTRUCTOR --- */

    MetalGraphicsPipeline::~MetalGraphicsPipeline()
    {
        renderPipelineState->release();
    }
    
    /* --- CONVERSIONS --- */

    MTL::CullMode MetalGraphicsPipeline::CullModeToCullMode(const CullMode cullMode)
    {
        switch (cullMode)
        {
            case CullMode::None:        return MTL::CullModeNone;
            case CullMode::Front:       return MTL::CullModeFront;
            case CullMode::Back:        return MTL::CullModeBack;
            default:                    break;
        }

        return MTL::CullModeNone;
    }

    MTL::TriangleFillMode MetalGraphicsPipeline::ShadeModeToTriangleFillMode(const ShadeMode shadeMode)
    {
        switch (shadeMode)
        {
            case ShadeMode::Fill:           return MTL::TriangleFillModeFill;
            case ShadeMode::Wireframe:      return MTL::TriangleFillModeLines;
            default:                        break;
        }

        return MTL::TriangleFillModeFill;
    }

    MTL::Winding MetalGraphicsPipeline::FrontFaceModeToWinding(const FrontFaceMode frontFaceMode)
    {
        switch (frontFaceMode)
        {
            case FrontFaceMode::Clockwise:              return MTL::WindingClockwise;
            case FrontFaceMode::CounterClockwise:       return MTL::WindingCounterClockwise;
            default:                                    break;
        }
        
        return MTL::WindingClockwise;
    }

}