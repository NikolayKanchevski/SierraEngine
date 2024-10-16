//
// Created by Nikolay Kanchevski on 11.12.23.
//

#include "CommandBuffer.h"

#include "Device.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    CommandBuffer::CommandBuffer(const CommandBufferCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create command buffer, as specified name must not be empty"));
    }
    
    /* --- POLLING METHODS --- */

    void CommandBuffer::SynchronizeBufferUsage(const Buffer& buffer, const BufferCommandUsage previousUsage, const BufferCommandUsage nextUsage, const size offset, const size memorySize)
    {
        SR_THROW_IF(nextUsage == BufferCommandUsage::None, InvalidValueError(SR_FORMAT("Cannot synchronize memory range of buffer [{0}] within command buffer [{1}], as specified next usage must not be [BufferCommandUsage::None]", buffer.GetName(), GetName())));
        SR_THROW_IF(offset + memorySize > buffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot synchronize invalid memory range of buffer [{0}] within command buffer [{1}]", buffer.GetName(), GetName()), offset, memorySize, size(0), buffer.GetMemorySize()));
    }

    void CommandBuffer::SynchronizeImageUsage(const Image& image, const ImageCommandUsage previousUsage, const ImageCommandUsage nextUsage, const uint32 baseLevel, const uint32 levelCount, const uint32 baseLayer, const uint32 layerCount)
    {
        SR_THROW_IF(baseLevel >= image.GetLevelCount(), ValueOutOfRangeError(SR_FORMAT("Cannot synchronize invalid level of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), baseLevel, 0U, image.GetLevelCount() - 1));
        SR_THROW_IF(baseLayer >= image.GetLayerCount(), ValueOutOfRangeError(SR_FORMAT("Cannot synchronize invalid layer of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), baseLayer, 0U, image.GetLayerCount() - 1));

        SR_THROW_IF(baseLevel + levelCount > image.GetLevelCount(), InvalidRangeError(SR_FORMAT("Cannot synchronize invalid levels of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), baseLevel, levelCount - 1, 0U, image.GetLevelCount()));
        SR_THROW_IF(baseLayer + layerCount > image.GetLayerCount(), InvalidRangeError(SR_FORMAT("Cannot synchronize invalid layers of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), baseLayer, layerCount - 1, 0U, image.GetLayerCount()));
    }

    void CommandBuffer::CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const size sourceOffset, const size destinationOffset, const size memorySize)
    {
        SR_THROW_IF(sourceOffset + memorySize > sourceBuffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot copy invalid memory range from buffer [{0}] within command buffer [{1}]", sourceBuffer.GetName(), GetName()), sourceOffset, memorySize, size(0), sourceBuffer.GetMemorySize()));
        SR_THROW_IF(destinationOffset + memorySize > destinationBuffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot copy to invalid memory range of buffer [{0}] within command buffer [{1}]", sourceBuffer.GetName(), GetName()), destinationOffset, memorySize, size(0), destinationBuffer.GetMemorySize()));
    }

    void CommandBuffer::CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const uint32 level, const uint32 layer, const size sourceOffset, const Vector3UInt destinationPixelOffset, const Vector3UInt pixelRange)
    {
        SR_THROW_IF(level >= destinationImage.GetLevelCount(), ValueOutOfRangeError(SR_FORMAT("Cannot copy to invalid level of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), level, 0U, destinationImage.GetLevelCount() - 1));
        SR_THROW_IF(layer >= destinationImage.GetLayerCount(), ValueOutOfRangeError(SR_FORMAT("Cannot copy to invalid layer of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), layer, 0U, destinationImage.GetLayerCount() - 1));

        SR_THROW_IF(destinationPixelOffset.x + pixelRange.x > (destinationImage.GetWidth() >> level), InvalidRangeError(SR_FORMAT("Cannot copy to invalid pixel range of the horizontal axis of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), destinationPixelOffset.x, static_cast<uint32>(pixelRange.x), 0U, destinationImage.GetWidth()));
        SR_THROW_IF(destinationPixelOffset.y + pixelRange.y > (destinationImage.GetHeight() >> level), InvalidRangeError(SR_FORMAT("Cannot copy to invalid pixel range of the vertical axis of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), destinationPixelOffset.y, static_cast<uint32>(pixelRange.y), 0U, destinationImage.GetHeight()));
        SR_THROW_IF(destinationPixelOffset.z + pixelRange.z > (destinationImage.GetDepth() >> level), InvalidRangeError(SR_FORMAT("Cannot copy to invalid pixel range of the depth axis of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), destinationPixelOffset.y, static_cast<uint32>(pixelRange.z), 0U, destinationImage.GetDepth()));
    }

    void CommandBuffer::GenerateMipMapsForImage(const Image& image)
    {
        
    }

    void CommandBuffer::BindResourceTable(const ResourceTable& resourceTable)
    {

    }

    void CommandBuffer::PushConstants(const void* memory, const size offset, const size memorySize)
    {
        SR_THROW_IF(memory == nullptr, InvalidValueError(SR_FORMAT("Cannot push push constant memory range [{0}-{1}] within command buffer [{2}], as specified memory pointer must not be null", GetName(), size(0), memorySize)));
        SR_THROW_IF(offset + memorySize > Device::MAX_PUSH_CONSTANT_SIZE, InvalidRangeError(SR_FORMAT("Cannot push invalid push constant range within command buffer [{0}]", GetName()), offset, memorySize, size(0), Device::MAX_PUSH_CONSTANT_SIZE));
    }

    void CommandBuffer::BeginRenderPass(const RenderPass& renderPass, const std::span<const RenderPassBeginAttachment> attachments)
    {
        SR_THROW_IF(renderPass.GetAttachmentCount() != attachments.size(), UnexpectedSizeError(SR_FORMAT("Cannot begin render pass [{0}] within command buffer [{1}], as specified attachments' count does not match that of the render pass", renderPass.GetName(), GetName()), attachments.size(), renderPass.GetAttachmentCount()));
    }

    void CommandBuffer::BeginNextSubpass(const RenderPass& renderPass)
    {

    }

    void CommandBuffer::EndRenderPass(const RenderPass& renderPass)
    {

    }

    void CommandBuffer::BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {

    }

    void CommandBuffer::EndGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {

    }

    void CommandBuffer::BindVertexBuffer(const Buffer& vertexBuffer, const size offset)
    {
        SR_THROW_IF(offset >= vertexBuffer.GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot bind invalid offset of vertex buffer [{0}] within command buffer [{1}]", vertexBuffer.GetName(), GetName()), offset, size(0), vertexBuffer.GetMemorySize()));
    }

    void CommandBuffer::BindIndexBuffer(const Buffer& indexBuffer, const size offset)
    {
        SR_THROW_IF(offset >= indexBuffer.GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot bind invalid offset of index buffer [{0}] within command buffer [{1}]", indexBuffer.GetName(), GetName()), offset, size(0), indexBuffer.GetMemorySize()));
    }

    void CommandBuffer::SetScissor(const Vector4UInt scissor)
    {

    }

    void CommandBuffer::Draw(const uint32 vertexCount, const size vertexOffset)
    {
        SR_THROW_IF(vertexCount == 0, InvalidValueError(SR_FORMAT("Cannot draw within command buffer [{0}], as specified vertex count be greater than [0]", GetName())));
    }

    void CommandBuffer::DrawIndexed(const uint32 indexCount, const size indexOffset, const size vertexOffset)
    {
        SR_THROW_IF(indexCount == 0, InvalidValueError(SR_FORMAT("Cannot draw indexed within command buffer [{0}], as specified index count be greater than [0]", GetName())));
    }

    void CommandBuffer::BeginComputePipeline(const ComputePipeline& computePipeline)
    {

    }

    void CommandBuffer::EndComputePipeline(const ComputePipeline& computePipeline)
    {

    }

    void CommandBuffer::Dispatch(const Vector3UInt workGroupSize)
    {
        SR_THROW_IF(workGroupSize.x == 0, InvalidValueError(SR_FORMAT("Cannot dispatch within command buffer [{0}], as specified horizontal work group size must be greater than [0]", GetName())));
    }

    void CommandBuffer::BeginDebugRegion(const std::string_view regionName, const Color32 color)
    {
        SR_THROW_IF(regionName.empty(), InvalidValueError(SR_FORMAT("Cannot begin debug region within command buffer [{0}], as specified region name must not be empty", GetName())));
    }

    void CommandBuffer::InsertDebugMarker(const std::string_view markerName, const Color32 color)
    {
        SR_THROW_IF(markerName.empty(), InvalidValueError(SR_FORMAT("Cannot insert debug marker within command buffer [{0}], as specified marker name must not be empty", GetName())));
    }

    void CommandBuffer::EndDebugRegion()
    {

    }


}