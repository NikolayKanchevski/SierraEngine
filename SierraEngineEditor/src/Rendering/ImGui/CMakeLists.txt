set(IMGUI_DIRECTORY_PATH ../../../vendor/imgui)
set(IMGUI_SOURCE_FILES
    ${IMGUI_DIRECTORY_PATH}/imgui.h
    ${IMGUI_DIRECTORY_PATH}/imgui.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_demo.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_internal.h
    ${IMGUI_DIRECTORY_PATH}/imgui_draw.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_tables.cpp
    ${IMGUI_DIRECTORY_PATH}/imgui_widgets.cpp
    ${IMGUI_DIRECTORY_PATH}/imstb_rectpack.h
    ${IMGUI_DIRECTORY_PATH}/imstb_truetype.h
    ${IMGUI_DIRECTORY_PATH}/misc/cpp/imgui_stdlib.cpp
    ${IMGUI_DIRECTORY_PATH}/misc/cpp/imgui_stdlib.h
)
target_sources(SierraEngineEditor PRIVATE ${IMGUI_SOURCE_FILES})
target_include_directories(SierraEngineEditor PRIVATE ${IMGUI_DIRECTORY_PATH} ${IMGUI_DIRECTORY_PATH}/misc/cpp)
target_compile_definitions(SierraEngineEditor PRIVATE "IMGUI_USER_CONFIG=\"../../src/Rendering/ImGui/ImGuiConfiguration.h\"")

target_sources(SierraEngineEditor PRIVATE
    ImGuiConfiguration.h
    ImGuiRenderer.h
    ImGuiRenderer.cpp
)