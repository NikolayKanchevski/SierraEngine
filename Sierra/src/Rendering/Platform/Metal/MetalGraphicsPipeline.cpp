//
// Created by Nikolay Kanchevski on 1.01.24.
//

#include "MetalGraphicsPipeline.h"

#include "MetalShader.h"
#include "MetalRenderPass.h"
#include "MetalCommandBuffer.h"
#include "MetalBuffer.h"
#include "MetalImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalGraphicsPipeline::MetalGraphicsPipeline(const MetalDevice &device, const Sierra::GraphicsPipelineCreateInfo &createInfo)
        : GraphicsPipeline(createInfo), MetalResource(createInfo.name), pipelineLayout(static_cast<MetalPipelineLayout&>(*createInfo.layout)), cullMode(CullModeToCullMode(createInfo.cullMode)), triangleFillMode(ShadeModeToTriangleFillMode(createInfo.shadeMode)), frontFacingWinding(FrontFaceModeToWinding(createInfo.frontFaceMode))
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create pipeline [{0}] using pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Metal]!");

        SR_ERROR_IF(createInfo.vertexShader->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.vertexShader->GetName());
        const MetalShader &metalVertexShader = static_cast<MetalShader&>(*createInfo.vertexShader);

        SR_ERROR_IF(createInfo.renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.renderPass->GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*createInfo.renderPass);

        // Configure pipeline descriptor's shaders
        MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        MTL_SET_OBJECT_NAME(renderPipelineDescriptor, GetName());
        renderPipelineDescriptor->setVertexFunction(metalVertexShader.GetEntryFunction());
        if (createInfo.fragmentShader.has_value())
        {
            SR_ERROR_IF(createInfo.fragmentShader.value().get()->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.fragmentShader.value().get()->GetName());
            const MetalShader &metalFragmentShader = static_cast<MetalShader&>(*createInfo.fragmentShader.value().get());

            renderPipelineDescriptor->setFragmentFunction(metalFragmentShader.GetEntryFunction());
            hasFragmentShader = true;
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
        uint32 vertexDataSize = 0;
        MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        for (uint32 i = 0; i < createInfo.vertexInputs.size(); i++)
        {
            vertexDescriptor->attributes()->object(i)->setBufferIndex(VERTEX_BUFFER_INDEX);
            vertexDescriptor->attributes()->object(i)->setOffset(vertexDataSize);
            switch (*(createInfo.vertexInputs.begin() + i))
            {
                case VertexInput::Float:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat);
                    vertexDataSize += sizeof(float32) * 1;
                    break;
                }
                case VertexInput::Float2:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat2);
                    vertexDataSize += sizeof(float32) * 2;
                    break;
                }
                case VertexInput::Float3:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat3);
                    vertexDataSize += sizeof(float32) * 3;
                    break;
                }
                case VertexInput::Float4:
                {
                    vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat4);
                    vertexDataSize += sizeof(float32) * 4;
                    break;
                }
            }
        }
        vertexDescriptor->layouts()->object(VERTEX_BUFFER_INDEX)->setStride(vertexDataSize);
        renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

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

    void MetalGraphicsPipeline::PushConstants(std::unique_ptr<CommandBuffer> &commandBuffer, const void* data, const uint16 memoryRange, const uint16 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot push constants to pipeline [{0}] using command buffer [{1}] as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(memoryRange > pipelineLayout.GetPushConstantSize(), "[Metal]: Cannot push [{0}] bytes of push constant data to pipeline [{1}], as specified memory range is bigger than specified in the corresponding pipeline layout, which is [{2}] bytes!", memoryRange, GetName(), pipelineLayout.GetPushConstantSize());

        metalCommandBuffer.GetCurrentRenderEncoder()->setVertexBytes(data, memoryRange, pipelineLayout.GetPushConstantIndex());
        if (hasFragmentShader) metalCommandBuffer.GetCurrentRenderEncoder()->setFragmentBytes(data, memoryRange, pipelineLayout.GetPushConstantIndex());
    }

    void MetalGraphicsPipeline::BindBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 binding, const std::unique_ptr<Buffer> &buffer, const uint32 arrayIndex, const uint64 memoryRange, const uint64 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind buffer [{0}] to binding [{1}] within pipeline [{2}] using command buffer [{3}] as its graphics API differs from [GraphicsAPI::Metal]!", buffer->GetName(), binding, GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(buffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind buffer [{0}] to binding [{1}] within pipeline [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", buffer->GetName(), binding, GetName());
        const MetalBuffer &metalBuffer = static_cast<const MetalBuffer&>(*buffer);

        SR_ERROR_IF(offset + memoryRange > buffer->GetMemorySize(), "[Metal]: Cannot bind [{0}] bytes (offset by another [{1}] bytes) from buffer [{2}] to pipeline [{3}], as the resulting memory space of a total of [{4}] bytes is bigger than the size of the buffer - [{5}]!", memoryRange, offset, buffer->GetName(), GetName(), offset + memoryRange, buffer->GetMemorySize());

        metalCommandBuffer.GetCurrentRenderEncoder()->setVertexBuffer(metalBuffer.GetMetalBuffer(), offset, pipelineLayout.GetBindingIndex(binding, arrayIndex));
        if (hasFragmentShader) metalCommandBuffer.GetCurrentRenderEncoder()->setFragmentBuffer(metalBuffer.GetMetalBuffer(), offset, pipelineLayout.GetBindingIndex(binding, arrayIndex));
    }

    void MetalGraphicsPipeline::BindImage(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 binding, const std::unique_ptr<Image> &image, const uint32 arrayIndex) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind image [{0}] to binding [{1}] within pipeline [{2}] using command buffer [{3}] as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), binding, GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(image->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind image [{0}] to binding [{1}] within pipeline [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", image->GetName(), binding, GetName());
        const MetalImage &metalImage = static_cast<const MetalImage&>(*image);

        metalCommandBuffer.GetCurrentRenderEncoder()->setVertexTexture(metalImage.GetMetalTexture(), pipelineLayout.GetBindingIndex(binding, arrayIndex));
        if (hasFragmentShader) metalCommandBuffer.GetCurrentRenderEncoder()->setFragmentTexture(metalImage.GetMetalTexture(), pipelineLayout.GetBindingIndex(binding, arrayIndex));
    }

    void MetalGraphicsPipeline::BindVertexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &vertexBuffer, const uint64 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind vertex buffer [{0}] to graphics pipeline [{1}] using command buffer [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", vertexBuffer->GetName(), GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(vertexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind vertex buffer [{0}] to graphics pipeline [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", vertexBuffer->GetName(), GetName());
        const MetalBuffer &metalVertexBuffer = static_cast<const MetalBuffer&>(*vertexBuffer);

        SR_ERROR_IF(offset > vertexBuffer->GetMemorySize(), "[Metal]: Cannot bind vertex buffer [{0}] to graphics pipeline [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", vertexBuffer->GetName(), GetName(), offset, vertexBuffer->GetMemorySize());
        metalCommandBuffer.GetCurrentRenderEncoder()->setVertexBuffer(metalVertexBuffer.GetMetalBuffer(), offset, VERTEX_BUFFER_INDEX);
    }

    void MetalGraphicsPipeline::BindIndexBuffer(std::unique_ptr<CommandBuffer> &commandBuffer, const std::unique_ptr<Buffer> &indexBuffer, const uint64 offset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind index buffer [{0}] to graphics pipeline [{1}] using command buffer [{2}], as its graphics API differs from [GraphicsAPI::Metal]!", indexBuffer->GetName(), GetName(), commandBuffer->GetName());
        MetalCommandBuffer &metalCommandBuffer = static_cast<MetalCommandBuffer&>(*commandBuffer);

        SR_ERROR_IF(indexBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot bind index buffer [{0}] to graphics pipeline [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", indexBuffer->GetName(), GetName());
        const MetalBuffer &metalIndexBuffer = static_cast<const MetalBuffer&>(*indexBuffer);

        SR_ERROR_IF(offset > indexBuffer->GetMemorySize(), "[Metal]: Cannot bind index buffer [{0}] to graphics pipeline [{1}] using specified offset of [{2}] bytes, which is outside of the [{3}] bytes size of the size of the buffer!", indexBuffer->GetName(), GetName(), offset, indexBuffer->GetMemorySize());
        metalCommandBuffer.BindIndexBuffer(metalIndexBuffer.GetMetalBuffer(), offset);
    }
    
    void MetalGraphicsPipeline::Draw(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 vertexCount) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot issue a draw call on graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        metalCommandBuffer.GetCurrentRenderEncoder()->drawPrimitives(MTL::PrimitiveTypeTriangle, NS::UInteger(0), vertexCount);
    }

    void MetalGraphicsPipeline::DrawIndexed(std::unique_ptr<CommandBuffer> &commandBuffer, const uint32 indexCount, const uint64 indexOffset, const uint64 vertexOffset) const
    {
        SR_ERROR_IF(commandBuffer->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot issue an indexed draw call on graphics pipeline [{0}] using command buffer [{1}], as its graphics API differs from [GraphicsAPI::Vulkan]!", GetName(), commandBuffer->GetName());
        const MetalCommandBuffer &metalCommandBuffer = static_cast<const MetalCommandBuffer&>(*commandBuffer);

        metalCommandBuffer.GetCurrentRenderEncoder()->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt32, metalCommandBuffer.GetCurrentIndexBuffer(), metalCommandBuffer.GetCurrentIndexBufferOffset() + indexOffset, 1, static_cast<int32>(vertexOffset), 0);
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