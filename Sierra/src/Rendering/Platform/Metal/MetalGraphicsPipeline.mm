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
        MTLRenderPipelineDescriptor* const renderPipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
        device.SetResourceName(renderPipelineDescriptor, GetName());

        // Configure pipeline descriptor's shaders
        [renderPipelineDescriptor setVertexFunction: metalVertexShader.GetEntryFunction()];
        if (createInfo.fragmentShader.has_value() && createInfo.fragmentShader->get() != nil)
        {
            SR_ERROR_IF(createInfo.fragmentShader->get()->GetAPI() != GraphicsAPI::Metal, "[Metal]: Cannot create graphics pipeline [{0}] with fragment shader [{1}], as its graphics API differs from [GraphicsAPI::Metal]!", GetName(), createInfo.fragmentShader->get()->GetName());
            const MetalShader &metalFragmentShader = static_cast<MetalShader&>(*createInfo.fragmentShader->get());

            [renderPipelineDescriptor setFragmentFunction: metalFragmentShader.GetEntryFunction()];
            hasFragmentShader = true;
        }

        // Set sample count
        [renderPipelineDescriptor setRasterSampleCount: MetalImage::ImageSamplingToUInteger(createInfo.sampling)];

        // Configure blending & pixel formats
        for (uint32 i = 0; i < metalRenderPass.GetColorAttachmentCount(); i++)
        {
            MTLRenderPipelineColorAttachmentDescriptor* colorAttachment = renderPipelineDescriptor.colorAttachments[i];
            [colorAttachment setPixelFormat: metalRenderPass.GetSubpass(createInfo.subpassIndex).colorAttachments[i].texture.pixelFormat];
            [colorAttachment setBlendingEnabled: createInfo.blendMode != BlendMode::None];
            [colorAttachment setRgbBlendOperation: MTLBlendOperationAdd];
            [colorAttachment setSourceRGBBlendFactor: MTLBlendFactorSourceAlpha];
            [colorAttachment setDestinationRGBBlendFactor: MTLBlendFactorOneMinusSourceAlpha];
            [colorAttachment setAlphaBlendOperation: MTLBlendOperationAdd];
            [colorAttachment setSourceAlphaBlendFactor: MTLBlendFactorOne];
            [colorAttachment setDestinationAlphaBlendFactor: MTLBlendFactorOneMinusSourceAlpha];
        }
        if (metalRenderPass.HasDepthAttachment())
        {
            [renderPipelineDescriptor setDepthAttachmentPixelFormat: metalRenderPass.GetSubpass(createInfo.subpassIndex).depthAttachment.texture.pixelFormat];
        }

        // Set up vertex attributes
        MTLVertexDescriptor* const vertexDescriptor = [[MTLVertexDescriptor alloc] init];
        for (uint32 i = 0; i < createInfo.vertexInputs.size(); i++)
        {
            [vertexDescriptor.attributes[i] setBufferIndex: MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX];
            [vertexDescriptor.attributes[i] setOffset: vertexByteStride];
            switch (*(createInfo.vertexInputs.begin() + i))
            {
                case VertexInput::Int8:          { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar];             vertexByteStride += 1 * 1; break; }
                case VertexInput::UInt8:         { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar];            vertexByteStride += 1 * 1; break; }
                case VertexInput::Norm8:         { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatCharNormalized];   vertexByteStride += 1 * 1; break; }
                case VertexInput::UNorm8:        { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUCharNormalized];  vertexByteStride += 1 * 1; break; }
                case VertexInput::Int16:         { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort];            vertexByteStride += 1 * 2; break; }
                case VertexInput::UInt16:        { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort];           vertexByteStride += 1 * 2; break; }
                case VertexInput::Norm16:        { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShortNormalized];  vertexByteStride += 1 * 2; break; }
                case VertexInput::UNorm16:       { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShortNormalized]; vertexByteStride += 1 * 2; break; }
                case VertexInput::Float16:       { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatHalf];             vertexByteStride += 1 * 4; break; }
                case VertexInput::Int32:         { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatInt];              vertexByteStride += 1 * 4; break; }
                case VertexInput::UInt32:        { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUInt];             vertexByteStride += 1 * 4; break; }
                case VertexInput::Float32:       { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatFloat];            vertexByteStride += 1 * 4; break; }

                case VertexInput::Int8_2D:       { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar2];             vertexByteStride += 2 * 1; break; }
                case VertexInput::UInt8_2D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar2];            vertexByteStride += 2 * 1; break; }
                case VertexInput::Norm8_2D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar2Normalized];   vertexByteStride += 2 * 1; break; }
                case VertexInput::UNorm8_2D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar2Normalized];  vertexByteStride += 2 * 1; break; }
                case VertexInput::Int16_2D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort2];            vertexByteStride += 2 * 2; break; }
                case VertexInput::UInt16_2D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort2];           vertexByteStride += 2 * 2; break; }
                case VertexInput::Norm16_2D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort2Normalized];  vertexByteStride += 2 * 2; break; }
                case VertexInput::UNorm16_2D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort2Normalized]; vertexByteStride += 2 * 2; break; }
                case VertexInput::Float16_2D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatHalf2];             vertexByteStride += 2 * 2; break; }
                case VertexInput::Int32_2D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatInt2];              vertexByteStride += 2 * 4; break; }
                case VertexInput::UInt32_2D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUInt2];             vertexByteStride += 2 * 4; break; }
                case VertexInput::Float32_2D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatFloat2];            vertexByteStride += 2 * 4; break; }

                case VertexInput::Int8_3D:       { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar3];             vertexByteStride += 3 * 1; break; }
                case VertexInput::UInt8_3D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar3];            vertexByteStride += 3 * 1; break; }
                case VertexInput::Norm8_3D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar3Normalized];   vertexByteStride += 3 * 1; break; }
                case VertexInput::UNorm8_3D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar3Normalized];  vertexByteStride += 3 * 1; break; }
                case VertexInput::Int16_3D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort3];            vertexByteStride += 3 * 2; break; }
                case VertexInput::UInt16_3D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort3];           vertexByteStride += 3 * 2; break; }
                case VertexInput::Norm16_3D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort3Normalized];  vertexByteStride += 3 * 2; break; }
                case VertexInput::UNorm16_3D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort3Normalized]; vertexByteStride += 3 * 2; break; }
                case VertexInput::Float16_3D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatHalf3];             vertexByteStride += 3 * 2; break; }
                case VertexInput::Int32_3D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatInt3];              vertexByteStride += 3 * 4; break; }
                case VertexInput::UInt32_3D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUInt3];             vertexByteStride += 3 * 4; break; }
                case VertexInput::Float32_3D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatFloat3];            vertexByteStride += 3 * 4; break; }

                case VertexInput::Int8_4D:       { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar4];             vertexByteStride += 4 * 1; break; }
                case VertexInput::UInt8_4D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar4];            vertexByteStride += 4 * 1; break; }
                case VertexInput::Norm8_4D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatChar4Normalized];   vertexByteStride += 4 * 1; break; }
                case VertexInput::UNorm8_4D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUChar4Normalized];  vertexByteStride += 4 * 1; break; }
                case VertexInput::Int16_4D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort4];            vertexByteStride += 4 * 2; break; }
                case VertexInput::UInt16_4D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort4];           vertexByteStride += 4 * 2; break; }
                case VertexInput::Norm16_4D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatShort4Normalized];  vertexByteStride += 4 * 2; break; }
                case VertexInput::UNorm16_4D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUShort4Normalized]; vertexByteStride += 4 * 2; break; }
                case VertexInput::Float16_4D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatHalf4];             vertexByteStride += 4 * 2; break; }
                case VertexInput::Int32_4D:      { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatInt4];              vertexByteStride += 4 * 4; break; }
                case VertexInput::UInt32_4D:     { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatUInt4];             vertexByteStride += 4 * 4; break; }
                case VertexInput::Float32_4D:    { [vertexDescriptor.attributes[i] setFormat: MTLVertexFormatFloat4];            vertexByteStride += 4 * 4; break; }
            }
        }
        [vertexDescriptor.layouts[MetalPipelineLayout::VERTEX_BUFFER_SHADER_INDEX] setStride: vertexByteStride];
        [renderPipelineDescriptor setVertexDescriptor: vertexDescriptor];

        // Set depth testing
        if (metalRenderPass.HasDepthAttachment())
        {
            MTLDepthStencilDescriptor* const depthStencilDescriptor = [[MTLDepthStencilDescriptor alloc] init];
            device.SetResourceName(depthStencilDescriptor, "Depth Stencil state of Graphics Pipeline [" + GetName() + "]");
            [depthStencilDescriptor setDepthWriteEnabled: createInfo.depthMode == DepthMode::WriteDepth];
            [depthStencilDescriptor setDepthCompareFunction: MTLCompareFunctionLessEqual];

            depthStencilState = [device.GetMetalDevice() newDepthStencilStateWithDescriptor: depthStencilDescriptor];
            [depthStencilDescriptor release];
        }

        // Create pipeline
        NSError* error = nil;
        renderPipelineState = [device.GetMetalDevice() newRenderPipelineStateWithDescriptor: renderPipelineDescriptor error: &error];
        SR_ERROR_IF(error != nil, "[Metal]: Could not create graphics pipeline [{0}]! Error: {1}.", GetName(), error.description.UTF8String);

        [renderPipelineDescriptor release];
        [vertexDescriptor release];
    }
    
    /* --- DESTRUCTOR --- */

    MetalGraphicsPipeline::~MetalGraphicsPipeline()
    {
        [renderPipelineState release];
        if (depthStencilState != nil) [depthStencilState release];
    }
    
    /* --- CONVERSIONS --- */

    MTLCullMode MetalGraphicsPipeline::CullModeToCullMode(const CullMode cullMode)
    {
        switch (cullMode)
        {
            case CullMode::None:        return MTLCullModeNone;
            case CullMode::Front:       return MTLCullModeFront;
            case CullMode::Back:        return MTLCullModeBack;
        }

        return MTLCullModeNone;
    }

    MTLTriangleFillMode MetalGraphicsPipeline::ShadeModeToTriangleFillMode(const ShadeMode shadeMode)
    {
        switch (shadeMode)
        {
            case ShadeMode::Fill:           return MTLTriangleFillModeFill;
            case ShadeMode::Wireframe:      return MTLTriangleFillModeLines;
        }

        return MTLTriangleFillModeFill;
    }

    MTLWinding MetalGraphicsPipeline::FrontFaceModeToWinding(const FrontFaceMode frontFaceMode)
    {
        switch (frontFaceMode)
        {
            case FrontFaceMode::Clockwise:              return MTLWindingClockwise;
            case FrontFaceMode::CounterClockwise:       return MTLWindingCounterClockwise;
        }
        
        return MTLWindingClockwise;
    }

}