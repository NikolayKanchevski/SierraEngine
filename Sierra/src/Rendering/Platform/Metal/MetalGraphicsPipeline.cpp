//
// Created by Nikolay Kanchevski on 1.01.24.
//

#include "MetalGraphicsPipeline.h"

#include "MetalShader.h"
#include "MetalRenderPass.h"
#include "MetalImage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MetalGraphicsPipeline::MetalGraphicsPipeline(const MetalDevice &device, const GraphicsPipelineCreateInfo &createInfo)
        : GraphicsPipeline(createInfo), MetalResource(createInfo.name), layout(static_cast<MetalPipelineLayout&>(*createInfo.layout)), cullMode(CullModeToCullMode(createInfo.cullMode)), triangleFillMode(ShadeModeToTriangleFillMode(createInfo.shadeMode)), winding(FrontFaceModeToWinding(createInfo.frontFaceMode))
    {
        SR_ERROR_IF(createInfo.layout->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] using pipeline layout [{1}], as its graphics API differs from [GraphicsAPI::Metal]!");

        SR_ERROR_IF(createInfo.vertexShader->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with vertex shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.vertexShader->GetName());
        const MetalShader &metalVertexShader = static_cast<MetalShader&>(*createInfo.vertexShader);

        SR_ERROR_IF(createInfo.templateRenderPass->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with render pass [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.templateRenderPass->GetName());
        const MetalRenderPass &metalRenderPass = static_cast<MetalRenderPass&>(*createInfo.templateRenderPass);

        // Allocate pipeline descriptor
        MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        device.SetResourceName(renderPipelineDescriptor, GetName());

        // Configure pipeline descriptor's shaders
        renderPipelineDescriptor->setVertexFunction(metalVertexShader.GetEntryFunction());
        if (createInfo.fragmentShader.has_value() && createInfo.fragmentShader->get() != nullptr)
        {
            SR_ERROR_IF(createInfo.fragmentShader->get()->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.fragmentShader->get()->GetName());
            const MetalShader &metalFragmentShader = static_cast<MetalShader&>(*createInfo.fragmentShader->get());

            renderPipelineDescriptor->setFragmentFunction(metalFragmentShader.GetEntryFunction());
            hasFragmentShader = true;
        }

        // Set sample count
        renderPipelineDescriptor->setSampleCount(MetalImage::ImageSamplingToUInteger(createInfo.sampling));

        // Configure blending & pixel formats
        for (uint32 i = 0; i < metalRenderPass.GetColorAttachmentCount(); i++)
        {
            MTL::RenderPipelineColorAttachmentDescriptor* colorAttachment = renderPipelineDescriptor->colorAttachments()->object(i);
            colorAttachment->setPixelFormat(metalRenderPass.GetSubpass(createInfo.subpassIndex)->colorAttachments()->object(i)->texture()->pixelFormat());
            colorAttachment->setBlendingEnabled(createInfo.blendMode != BlendMode::None);
            colorAttachment->setRgbBlendOperation(MTL::BlendOperationAdd);
            colorAttachment->setSourceRGBBlendFactor(MTL::BlendFactorSourceAlpha);
            colorAttachment->setDestinationRGBBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
            colorAttachment->setAlphaBlendOperation(MTL::BlendOperationAdd);
            colorAttachment->setSourceAlphaBlendFactor(MTL::BlendFactorOne);
            colorAttachment->setDestinationAlphaBlendFactor(MTL::BlendFactorOneMinusSourceAlpha);
        }
        if (metalRenderPass.HasDepthAttachment())
        {
            renderPipelineDescriptor->setDepthAttachmentPixelFormat(metalRenderPass.GetSubpass(createInfo.subpassIndex)->depthAttachment()->texture()->pixelFormat());
        }

        // Set up vertex attributes
        MTL::VertexDescriptor* vertexDescriptor = MTL::VertexDescriptor::alloc()->init();
        for (uint32 i = 0; i < createInfo.vertexInputs.size(); i++)
        {
            vertexDescriptor->attributes()->object(i)->setBufferIndex(MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX);
            vertexDescriptor->attributes()->object(i)->setOffset(vertexByteStride);
            switch (*(createInfo.vertexInputs.begin() + i))
            {
                case VertexInput::Int8:          { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar);             vertexByteStride += 1 * 1; break; }
                case VertexInput::UInt8:         { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar);            vertexByteStride += 1 * 1; break; }
                case VertexInput::Norm8:         { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatCharNormalized);   vertexByteStride += 1 * 1; break; }
                case VertexInput::UNorm8:        { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUCharNormalized);  vertexByteStride += 1 * 1; break; }
                case VertexInput::Int16:         { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort);            vertexByteStride += 1 * 2; break; }
                case VertexInput::UInt16:        { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort);           vertexByteStride += 1 * 2; break; }
                case VertexInput::Norm16:        { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShortNormalized);  vertexByteStride += 1 * 2; break; }
                case VertexInput::UNorm16:       { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShortNormalized); vertexByteStride += 1 * 2; break; }
                case VertexInput::Float16:       { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatHalf);             vertexByteStride += 1 * 4; break; }
                case VertexInput::Int32:         { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatInt);              vertexByteStride += 1 * 4; break; }
                case VertexInput::UInt32:        { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUInt);             vertexByteStride += 1 * 4; break; }
                case VertexInput::Float32:       { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat);            vertexByteStride += 1 * 4; break; }

                case VertexInput::Int8_2D:       { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar2);             vertexByteStride += 2 * 1; break; }
                case VertexInput::UInt8_2D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar2);            vertexByteStride += 2 * 1; break; }
                case VertexInput::Norm8_2D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar2Normalized);   vertexByteStride += 2 * 1; break; }
                case VertexInput::UNorm8_2D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar2Normalized);  vertexByteStride += 2 * 1; break; }
                case VertexInput::Int16_2D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort2);            vertexByteStride += 2 * 2; break; }
                case VertexInput::UInt16_2D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort2);           vertexByteStride += 2 * 2; break; }
                case VertexInput::Norm16_2D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort2Normalized);  vertexByteStride += 2 * 2; break; }
                case VertexInput::UNorm16_2D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort2Normalized); vertexByteStride += 2 * 2; break; }
                case VertexInput::Float16_2D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatHalf2);             vertexByteStride += 2 * 2; break; }
                case VertexInput::Int32_2D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatInt2);              vertexByteStride += 2 * 4; break; }
                case VertexInput::UInt32_2D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUInt2);             vertexByteStride += 2 * 4; break; }
                case VertexInput::Float32_2D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat2);            vertexByteStride += 2 * 4; break; }

                case VertexInput::Int8_3D:       { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar3);             vertexByteStride += 3 * 1; break; }
                case VertexInput::UInt8_3D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar3);            vertexByteStride += 3 * 1; break; }
                case VertexInput::Norm8_3D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar3Normalized);   vertexByteStride += 3 * 1; break; }
                case VertexInput::UNorm8_3D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar3Normalized);  vertexByteStride += 3 * 1; break; }
                case VertexInput::Int16_3D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort3);            vertexByteStride += 3 * 2; break; }
                case VertexInput::UInt16_3D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort3);           vertexByteStride += 3 * 2; break; }
                case VertexInput::Norm16_3D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort3Normalized);  vertexByteStride += 3 * 2; break; }
                case VertexInput::UNorm16_3D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort3Normalized); vertexByteStride += 3 * 2; break; }
                case VertexInput::Float16_3D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatHalf3);             vertexByteStride += 3 * 2; break; }
                case VertexInput::Int32_3D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatInt3);              vertexByteStride += 3 * 4; break; }
                case VertexInput::UInt32_3D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUInt3);             vertexByteStride += 3 * 4; break; }
                case VertexInput::Float32_3D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat3);            vertexByteStride += 3 * 4; break; }

                case VertexInput::Int8_4D:       { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar4);             vertexByteStride += 4 * 1; break; }
                case VertexInput::UInt8_4D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar4);            vertexByteStride += 4 * 1; break; }
                case VertexInput::Norm8_4D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatChar4Normalized);   vertexByteStride += 4 * 1; break; }
                case VertexInput::UNorm8_4D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUChar4Normalized);  vertexByteStride += 4 * 1; break; }
                case VertexInput::Int16_4D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort4);            vertexByteStride += 4 * 2; break; }
                case VertexInput::UInt16_4D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort4);           vertexByteStride += 4 * 2; break; }
                case VertexInput::Norm16_4D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatShort4Normalized);  vertexByteStride += 4 * 2; break; }
                case VertexInput::UNorm16_4D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUShort4Normalized); vertexByteStride += 4 * 2; break; }
                case VertexInput::Float16_4D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatHalf4);             vertexByteStride += 4 * 2; break; }
                case VertexInput::Int32_4D:      { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatInt4);              vertexByteStride += 4 * 4; break; }
                case VertexInput::UInt32_4D:     { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatUInt4);             vertexByteStride += 4 * 4; break; }
                case VertexInput::Float32_4D:    { vertexDescriptor->attributes()->object(i)->setFormat(MTL::VertexFormatFloat4);            vertexByteStride += 4 * 4; break; }
            }
        }
        vertexDescriptor->layouts()->object(MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX)->setStride(vertexByteStride);
        renderPipelineDescriptor->setVertexDescriptor(vertexDescriptor);

        // Set depth testing
        if (metalRenderPass.HasDepthAttachment())
        {
            MTL::DepthStencilDescriptor* depthStencilDescriptor = MTL::DepthStencilDescriptor::alloc()->init();
            device.SetResourceName(depthStencilDescriptor, "Depth Stencil state of Graphics Pipeline [" + GetName() + "]");
            depthStencilDescriptor->setDepthWriteEnabled(createInfo.depthMode == DepthMode::WriteDepth);
            depthStencilDescriptor->setDepthCompareFunction(MTL::CompareFunctionLessEqual);

            depthStencilState = device.GetMetalDevice()->newDepthStencilState(depthStencilDescriptor);
            depthStencilDescriptor->release();
        }


        // Create pipeline
        NS::Error* error = nullptr;
        renderPipelineState = device.GetMetalDevice()->newRenderPipelineState(renderPipelineDescriptor, &error);
        SR_ERROR_IF(error != nullptr, "[Metal]: Could not create graphics pipeline [{0}]! Error: {1}.", GetName(), error->description()->cString(NS::ASCIIStringEncoding));

        renderPipelineDescriptor->release();
        vertexDescriptor->release();
    }
    
    /* --- DESTRUCTOR --- */

    MetalGraphicsPipeline::~MetalGraphicsPipeline()
    {
        renderPipelineState->release();
        if (depthStencilState != nullptr) depthStencilState->release();
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