# Link source files
target_sources(Sierra PRIVATE
    Buffer.cpp
    Buffer.h
    CommandBuffer.cpp
    CommandBuffer.h
    CommandTask.cpp
    CommandTask.h
    ComputePipeline.cpp
    ComputePipeline.h
    Device.cpp
    Device.h
    GraphicsPipeline.cpp
    GraphicsPipeline.h
    Image.cpp
    Image.h
        ImGuiRenderTask.cpp
        ImGuiRenderTask.h
    PipelineLayout.cpp
    PipelineLayout.h
    Renderer.cpp
    Renderer.h
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

set(IMGUI_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../vendor/ImGui)
target_sources(Sierra PRIVATE
    ${IMGUI_DIRECTORY}/imgui.cpp
    ${IMGUI_DIRECTORY}/imgui.h
    ${IMGUI_DIRECTORY}/imconfig.h
    ${IMGUI_DIRECTORY}/imgui_demo.cpp
    ${IMGUI_DIRECTORY}/imgui_internal.h
    ${IMGUI_DIRECTORY}/imgui_draw.cpp
    ${IMGUI_DIRECTORY}/imgui_tables.cpp
    ${IMGUI_DIRECTORY}/imgui_widgets.cpp
    ${IMGUI_DIRECTORY}/imstb_rectpack.h
    ${IMGUI_DIRECTORY}/imstb_textedit.h
    ${IMGUI_DIRECTORY}/imstb_truetype.h
)
target_include_directories(Sierra PRIVATE ${IMGUI_DIRECTORY})

# Link platform-specific source files and libraries
if(SIERRA_BUILD_VULKAN)
    add_subdirectory(Platform/Vulkan)

    target_sources(Sierra PRIVATE
        ${IMGUI_DIRECTORY}/backends/imgui_impl_vulkan.cpp
        ${IMGUI_DIRECTORY}/backends/imgui_impl_vulkan.h
    )
endif()
if(SIERRA_BUILD_METAL)
    add_subdirectory(Platform/Metal)

    target_sources(Sierra PRIVATE
        ${IMGUI_DIRECTORY}/backends/imgui_impl_metal.mm
        ${IMGUI_DIRECTORY}/backends/imgui_impl_metal.h
    )
endif()