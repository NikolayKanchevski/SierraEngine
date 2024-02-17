# Set Metal's prerequisites (Metal is guaranteed to be found)
set(Metal_LIBRARIES
    "-framework CoreGraphics"
    "-framework Metal"
    "-framework QuartzCore"
)
message(STATUS "[Sierra]: Building Metal...")

# Link graphics libraries
target_link_libraries(Sierra PRIVATE ${Metal_LIBRARIES})

# Set Metal definitions
target_compile_definitions(Sierra PUBLIC "SR_METAL_SUPPORTED")

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
    MetalPipelineLayout.mm
    MetalPipelineLayout.h
    MetalRenderPass.mm
    MetalRenderPass.h
    MetalResource.mm
    MetalSampler.h
    MetalSampler.mm
    MetalShader.mm
    MetalShader.h
    MetalSwapchain.mm
    MetalSwapchain.h
)