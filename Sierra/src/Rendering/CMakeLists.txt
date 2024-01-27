# Link source files
target_sources(Sierra PRIVATE
    Buffer.cpp
    Buffer.h
    CommandBuffer.cpp
    CommandBuffer.h
    ComputePipeline.cpp
    ComputePipeline.h
    Device.cpp
    Device.h
    GraphicsPipeline.cpp
    GraphicsPipeline.h
    Image.cpp
    Image.h
    PipelineLayout.cpp
    PipelineLayout.h
    RenderingContext.cpp
    RenderingContext.h
    RenderingResource.h
    RenderPass.cpp
    RenderPass.h
    Sampler.cpp
    Sampler.h
    Shader.cpp
    Shader.h
    Swapchain.cpp
    Swapchain.h
)

# Link platform-specific source files and libraries
if(SIERRA_BUILD_VULKAN)
    add_subdirectory(Platform/Vulkan)
endif()
if(SIERRA_BUILD_METAL)
    add_subdirectory(Platform/Metal)
endif()