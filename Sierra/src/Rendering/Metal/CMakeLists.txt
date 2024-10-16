# Set Metal's prerequisites
set(Metal_LIBRARIES
    "-framework CoreGraphics"
    "-framework Metal"
    "-framework QuartzCore"
)
message(STATUS "[Sierra]: Building Metal...")

# Link Metal's dependencies
target_link_libraries(Sierra PRIVATE ${Metal_LIBRARIES})

# Set Metal definitions
target_compile_definitions(Sierra PUBLIC "SR_METAL_SUPPORTED=1")

# Add Metal source files
target_sources(Sierra PRIVATE
    MetalBuffer.mm
    MetalBuffer.h
    MetalCommandBuffer.mm
    MetalCommandBuffer.h
    MetalComputePipeline.mm
    MetalComputePipeline.h
    MetalContext.mm
    MetalContext.h
    MetalDevice.mm
    MetalDevice.h
    MetalGraphicsPipeline.mm
    MetalGraphicsPipeline.h
    MetalImage.mm
    MetalImage.h
    MetalQueue.mm
    MetalQueue.h
    MetalRenderPass.mm
    MetalRenderPass.h
    MetalResource.h
    MetalResourceTable.mm
    MetalResourceTable.h
    MetalSampler.h
    MetalSampler.mm
    MetalShader.mm
    MetalShader.h
    MetalSwapchain.mm
    MetalSwapchain.h
)