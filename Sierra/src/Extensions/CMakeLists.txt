# Build enabled extensions
if(SIERRA_BUILD_IMGUI)
    target_sources(Sierra PRIVATE
        ImGuiRenderTask.cpp
        ImGuiRenderTask.h
    )

    add_library(ImGui STATIC)
    set_target_properties(ImGui PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    set(IMGUI_DIRECTORY ../../vendor/ImGui)
    target_sources(ImGui PRIVATE
        ${IMGUI_DIRECTORY}/imgui.cpp
        ${IMGUI_DIRECTORY}/imgui.h
        ${IMGUI_DIRECTORY}/imconfig.h
        ${IMGUI_DIRECTORY}/imgui_demo.cpp
        ${IMGUI_DIRECTORY}/imgui_internal.h
        ${IMGUI_DIRECTORY}/imgui_draw.cpp
        ${IMGUI_DIRECTORY}/imgui_tables.cpp
        ${IMGUI_DIRECTORY}/imgui_widgets.cpp
        ${IMGUI_DIRECTORY}/imstb_rectpack.h
        ${IMGUI_DIRECTORY}/imstb_truetype.h
    )
    target_compile_definitions(ImGui PRIVATE "ImDrawIdx=uint32_t")

    target_include_directories(Sierra PUBLIC ${IMGUI_DIRECTORY})
    target_link_libraries(Sierra PUBLIC ImGui)

    target_compile_definitions(Sierra PUBLIC SR_BUILD_IMGUI)
endif()