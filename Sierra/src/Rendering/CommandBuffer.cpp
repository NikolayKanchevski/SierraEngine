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

    void CommandBuffer::SynchronizeBufferUsage(const Buffer& buffer, const BufferSynchronizeInfo& synchronizeInfo)
    {
        SR_THROW_IF(synchronizeInfo.nextUsage == BufferCommandUsage::None, InvalidValueError(SR_FORMAT("Cannot synchronize memory range of buffer [{0}] within command buffer [{1}], as specified next usage must not be [BufferCommandUsage::None]", buffer.GetName(), GetName())));
        SR_THROW_IF(synchronizeInfo.offset + synchronizeInfo.memorySize > buffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot synchronize invalid memory range of buffer [{0}] within command buffer [{1}]", buffer.GetName(), GetName()), synchronizeInfo.offset, synchronizeInfo.memorySize, static_cast<uint64>(0), buffer.GetMemorySize()));
    }

    void CommandBuffer::SynchronizeImageUsage(const Image& image, const ImageSynchronizeInfo& synchronizeInfo)
    {
        SR_THROW_IF(synchronizeInfo.baseLevel >= image.GetLevelCount(), ValueOutOfRangeError(SR_FORMAT("Cannot synchronize invalid level of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), synchronizeInfo.baseLevel, uint32(0), image.GetLevelCount() - 1));
        SR_THROW_IF(synchronizeInfo.baseLayer >= image.GetLayerCount(), ValueOutOfRangeError(SR_FORMAT("Cannot synchronize invalid layer of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), synchronizeInfo.baseLayer, uint32(0), image.GetLayerCount() - 1));

        SR_THROW_IF(synchronizeInfo.baseLevel + synchronizeInfo.levelCount > image.GetLevelCount(), InvalidRangeError(SR_FORMAT("Cannot synchronize invalid levels of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), synchronizeInfo.baseLevel, synchronizeInfo.levelCount - 1, uint32(0), image.GetLevelCount()));
        SR_THROW_IF(synchronizeInfo.baseLayer + synchronizeInfo.layerCount > image.GetLayerCount(), InvalidRangeError(SR_FORMAT("Cannot synchronize invalid layers of image [{0}] within command buffer [{1}]", image.GetName(), GetName()), synchronizeInfo.baseLayer, synchronizeInfo.layerCount - 1, uint32(0), image.GetLayerCount()));
    }

    void CommandBuffer::CopyBufferToBuffer(const Buffer& sourceBuffer, const Buffer& destinationBuffer, const BufferToBufferCopyInfo& copyInfo)
    {
        SR_THROW_IF(copyInfo.sourceOffset + copyInfo.memorySize > sourceBuffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot copy invalid memory range from buffer [{0}] within command buffer [{1}]", sourceBuffer.GetName(), GetName()), copyInfo.sourceOffset, copyInfo.memorySize, static_cast<uint64>(0), sourceBuffer.GetMemorySize()));
        SR_THROW_IF(copyInfo.destinationOffset + copyInfo.memorySize > destinationBuffer.GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot copy to invalid memory range of buffer [{0}] within command buffer [{1}]", sourceBuffer.GetName(), GetName()), copyInfo.destinationOffset, copyInfo.memorySize, static_cast<uint64>(0), destinationBuffer.GetMemorySize()));
    }

    void CommandBuffer::CopyBufferToImage(const Buffer& sourceBuffer, const Image& destinationImage, const BufferToImageCopyInfo& copyInfo)
    {
        SR_THROW_IF(copyInfo.level >= destinationImage.GetLevelCount(), ValueOutOfRangeError(SR_FORMAT("Cannot copy to invalid level of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), copyInfo.level, uint32(0), destinationImage.GetLevelCount() - 1));
        SR_THROW_IF(copyInfo.layer >= destinationImage.GetLayerCount(), ValueOutOfRangeError(SR_FORMAT("Cannot copy to invalid layer of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), copyInfo.layer, uint32(0), destinationImage.GetLayerCount() - 1));

        SR_THROW_IF(copyInfo.destinationPixelOffset.x + copyInfo.pixelRange.x > glm::max(1U, destinationImage.GetWidth() >> copyInfo.level), InvalidRangeError(SR_FORMAT("Cannot copy to invalid pixel range of the horizontal axis of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), copyInfo.destinationPixelOffset.x, copyInfo.pixelRange.x, uint32(0), destinationImage.GetWidth()));
        SR_THROW_IF(copyInfo.destinationPixelOffset.y + copyInfo.pixelRange.y > glm::max(1U, destinationImage.GetHeight() >> copyInfo.level), InvalidRangeError(SR_FORMAT("Cannot copy to invalid pixel range of the vertical axis of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), copyInfo.destinationPixelOffset.y, copyInfo.pixelRange.y, uint32(0), destinationImage.GetHeight()));
        SR_THROW_IF(copyInfo.destinationPixelOffset.z + copyInfo.pixelRange.z > glm::max(1U, destinationImage.GetDepth() >> copyInfo.level), InvalidRangeError(SR_FORMAT("Cannot copy to invalid pixel range of the depth axis of image [{0}] within command buffer [{1}]", destinationImage.GetName(), GetName()), copyInfo.destinationPixelOffset.y, copyInfo.pixelRange.z, uint32(0), destinationImage.GetDepth()));
    }

    void CommandBuffer::GenerateMipMapsForImage(const Image& image)
    {
        
    }

    void CommandBuffer::BindResourceTable(const ResourceTable& resourceTable)
    {

    }

    void CommandBuffer::PushConstants(const void* memory, const uint8 offset, const uint8 memorySize)
    {
        SR_THROW_IF(memory == nullptr, InvalidValueError(SR_FORMAT("Cannot push push constant memory range [{0}-{1}] within command buffer [{2}], as specified memory pointer must not be null", GetName(), static_cast<uint64>(0), memorySize)));
        SR_THROW_IF(offset + memorySize > Device::MAX_PUSH_CONSTANT_SIZE, InvalidRangeError(SR_FORMAT("Cannot push invalid push constant range within command buffer [{0}]", GetName()), offset, memorySize, uint8(0), Device::MAX_PUSH_CONSTANT_SIZE));
    }

    void CommandBuffer::BeginRenderPass(const RenderPass& renderPass, const Framebuffer& framebuffer)
    {

    }

    void CommandBuffer::BeginNextSubpass()
    {

    }

    void CommandBuffer::EndRenderPass()
    {

    }

    void CommandBuffer::BeginGraphicsPipeline(const GraphicsPipeline& graphicsPipeline)
    {

    }

    void CommandBuffer::EndGraphicsPipeline()
    {

    }

    void CommandBuffer::BindVertexBuffer(const Buffer& vertexBuffer, const uint64 offset)
    {
        SR_THROW_IF(offset >= vertexBuffer.GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot bind invalid offset of vertex buffer [{0}] within command buffer [{1}]", vertexBuffer.GetName(), GetName()), offset, static_cast<uint64>(0), vertexBuffer.GetMemorySize()));
    }

    void CommandBuffer::BindIndexBuffer(const Buffer& indexBuffer, const uint64 offset)
    {
        SR_THROW_IF(offset >= indexBuffer.GetMemorySize(), ValueOutOfRangeError(SR_FORMAT("Cannot bind invalid offset of index buffer [{0}] within command buffer [{1}]", indexBuffer.GetName(), GetName()), offset, static_cast<uint64>(0), indexBuffer.GetMemorySize()));
    }

    void CommandBuffer::SetScissor(const Vector4UInt scissor)
    {

    }

    void CommandBuffer::Draw(const uint32 vertexCount, const uint64 vertexOffset)
    {
        SR_THROW_IF(vertexCount == 0, InvalidValueError(SR_FORMAT("Cannot draw within command buffer [{0}], as specified vertex count be greater than [0]", GetName())));
    }

    void CommandBuffer::DrawIndexed(const uint32 indexCount, const uint64 indexOffset, const uint64 vertexOffset)
    {
        SR_THROW_IF(indexCount == 0, InvalidValueError(SR_FORMAT("Cannot draw indexed within command buffer [{0}], as specified index count be greater than [0]", GetName())));
    }

    void CommandBuffer::BeginComputePipeline(const ComputePipeline& computePipeline)
    {

    }

    void CommandBuffer::EndComputePipeline()
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