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
        : GraphicsPipeline(createInfo), MetalResource(createInfo.name), layout(static_cast<MetalPipelineLayout&>(*createInfo.layout)), cullMode(CullModeToCullMode(createInfo.cullMode)), triangleFillMode(ShadeModeToTriangleFillMode(createInfo.shadeMode)), winding(FrontFaceModeToWinding(createInfo.frontFaceMode))
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create pipeline [{0}] using pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Metal]!");

        SR_ERROR_IF(createInfo.vertexShader->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.vertexShader->GetName());
        const MetalShader &metalVertexShader = static_cast<MetalShader&>(*createInfo.vertexShader);

        SR_ERROR_IF(createInfo.renderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.renderPass->GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*createInfo.renderPass);

        // Allocate pipeline descriptor
        MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        device.SetResourceName(renderPipelineDescriptor, GetName());

        // Configure pipeline descriptor's shaders
        renderPipelineDescriptor->setVertexFunction(metalVertexShader.GetEntryFunction());
        if (createInfo.fragmentShader.has_value())
        {
            SR_ERROR_IF(createInfo.fragmentShader->get()->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.fragmentShader->get()->GetName());
            const MetalShader &metalFragmentShader = static_cast<MetalShader&>(*createInfo.fragmentShader->get());

            renderPipelineDescriptor->setFragmentFunction(metalFragmentShader.GetEntryFunction());
            hasFragmentShader = true;
        }

        // Set sample count
        renderPipelineDescriptor->setSampleCount(MetalImage::ImageSamplingToUInteger(createInfo.sampling));

        // Configure pixel formats
        for (uint32 i = 0; i < metalRenderPass.GetColorAttachmentCount(); i++)
        {
            renderPipelineDescriptor->colorAttachments()->object(i)->setPixelFormat(metalRenderPass.GetSubpass(createInfo.subpassIndex)->colorAttachments()->object(i)->texture()->pixelFormat());
        }
        if (metalRenderPass.HasDepthAttachment())
        {
            renderPipelineDescriptor->setDepthAttachmentPixelFormat(metalRenderPass.GetSubpass(createInfo.subpassIndex)->depthAttachment()->texture()->pixelFormat());
        }

        // Set up vertex attributes
        uint32 vertexDataSize = 0;
        MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        for (uint32 i = 0; i < createInfo.vertexInputs.size(); i++)
        {
            vertexDescriptor->attributes()->object(i)->setBufferIndex(MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX);
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
        vertexDescriptor->layouts()->object(MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX)->setStride(vertexDataSize);
        renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

        // Create pipeline
        NS::Error* error = nullptr;
        renderPipelineState = device.GetMetalDevice()->newRenderPipelineState(renderPipelineDescriptor, &error);
        SR_ERROR_IF(error != nullptr, "Could not create Metal graphics pipeline [{0}]! Error: {1}.", GetName(), error->localizedDescription()->cString(NS::ASCIIStringEncoding));

        renderPipelineDescriptor->release();
        vertexDescriptor->release();
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
        }

        return MTL::CullModeNone;
    }

    MTL::TriangleFillMode MetalGraphicsPipeline::ShadeModeToTriangleFillMode(const ShadeMode shadeMode)
    {
        switch (shadeMode)
        {
            case ShadeMode::Fill:           return MTL::TriangleFillModeFill;
            case ShadeMode::Wireframe:      return MTL::TriangleFillModeLines;
        }

        return MTL::TriangleFillModeFill;
    }

    MTL::Winding MetalGraphicsPipeline::FrontFaceModeToWinding(const FrontFaceMode frontFaceMode)
    {
        switch (frontFaceMode)
        {
            case FrontFaceMode::Clockwise:              return MTL::WindingClockwise;
            case FrontFaceMode::CounterClockwise:       return MTL::WindingCounterClockwise;
        }
        
        return MTL::WindingClockwise;
    }

}