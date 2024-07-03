target_compile_definitions(Sierra PUBLIC "SR_BUILD_IMGUI_EXTENSION")
target_sources(Sierra PRIVATE
    ImGuiConfiguration.h
    ImGuiRenderer.h
    ImGuiRenderer.cpp
)

set(IMGUI_DIRECTORY_PATH ../../../vendor/ImGui/)
target_sources(Sierra PRIVATE
    ${IMGUI_DIRECTORY_PATH}/imgui.h
    ${IMGUI_DIRECTORY_PATH}/imgui.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_demo.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_internal.h
    ${IMGUI_DIRECTORY_PATH}/imgui_draw.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_tables.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_widgets.cpp
    ${IMGUI_DIRECTORY_PATH}/imstb_rectpack.h
    ${IMGUI_DIRECTORY_PATH}/imstb_truetype.h
)

target_include_directories(Sierra PUBLIC ${IMGUI_DIRECTORY_PATH})
target_compile_definitions(Sierra PUBLIC "IMGUI_USER_CONFIG=\"../../src/Extensions/ImGui/ImGuiConfiguration.h\"")