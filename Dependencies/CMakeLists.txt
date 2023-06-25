set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.20)

add_library(Dependencies INTERFACE)

# ========== KHRONOS UTILITIES ========== #
set(SPIRV_WERROR OFF)
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)

add_library(SPIRV-Reflect STATIC SPIRV-Reflect/spirv_reflect.h SPIRV-Reflect/spirv_reflect.c)
target_link_libraries(Dependencies INTERFACE SPIRV-Reflect)
target_include_directories(Dependencies INTERFACE SPIRV-Reflect/)

# ========== VOLK ========== #
add_subdirectory(Volk)
target_link_libraries(Dependencies INTERFACE volk::volk)
target_include_directories(Dependencies INTERFACE Volk/)

# ========== VMA ========== #
set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
add_subdirectory(VMA)
target_include_directories(Dependencies SYSTEM INTERFACE VMA/include/ -w)

# ========== SHADERC ========== #
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_ENABLE_TESTS OFF)

add_subdirectory(shaderc)
target_link_libraries(Dependencies INTERFACE shaderc)

# ========== GLFW ========== #
set(GLFW_BUILD_EXAMPLES, OFF)
set(GLFW_BUILD_TESTS, OFF)
set(GLFW_BUILD_DOCS, OFF)

add_subdirectory(GLFW)
target_link_libraries(Dependencies INTERFACE glfw)
target_include_directories(Dependencies INTERFACE GLFW/include/)

# ========== GLM ========== #
add_subdirectory(glm)
target_link_libraries(Dependencies INTERFACE glm)
target_include_directories(Dependencies INTERFACE glm)

# ========== IMGUI ========== #
add_library(IMGUI STATIC)
target_sources(IMGUI PRIVATE ImGui/imgui_demo.cpp ImGui/imgui_draw.cpp ImGui/imgui_tables.cpp ImGui/imgui_widgets.cpp ImGui/imgui.cpp ImGui/imgui.h ImGui/imgui_internal.h ImGui/imconfig.h ImGui/imstb_rectpack.h ImGui/imstb_textedit.h ImGui/imstb_truetype.h ImGui/backends/imgui_impl_vulkan.cpp ImGui/backends/imgui_impl_vulkan.h ImGui/backends/imgui_impl_glfw.cpp ImGui/backends/imgui_impl_glfw.h ImGui/misc/cpp/imgui_stdlib.h ImGui/misc/cpp/imgui_stdlib.cpp)
target_include_directories(IMGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/GLFW/include/ ${Vulkan_INCLUDE_DIR} ImGui/backends ImGui/ Volk/)
target_link_libraries(Dependencies INTERFACE IMGUI)
target_include_directories(Dependencies INTERFACE ImGui ImGui/backends ImGui/misc/cpp/)

# ========== IMGUIZMO ========== #
add_library(IMGUIZMO STATIC)
target_sources(IMGUIZMO PRIVATE ImGuizmo/ImGuizmo.cpp ImGuizmo/ImGuizmo.h)
target_include_directories(IMGUIZMO PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/)
target_link_libraries(Dependencies INTERFACE IMGUIZMO)
target_include_directories(Dependencies INTERFACE ImGuizmo)

# ========== ENTT ========== #
add_subdirectory(EnTT)
target_link_libraries(Dependencies INTERFACE EnTT)
target_include_directories(Dependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/EnTT/single_include/)

# ========== ASSIMP ========== #
set(ASSIMP_NO_EXPORT ON)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(ASSIMP_WARNINGS_AS_ERRORS OFF)
else()
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
endif()

add_subdirectory(Assimp)
target_link_libraries(Dependencies INTERFACE assimp)
target_include_directories(Dependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Assimp/include/)

# ========== STB ========== #
target_include_directories(Dependencies INTERFACE stb)

# ========== FMT ========== #
set(FMT_DOC OFF)
set(FMT_TEST OFF)

add_subdirectory(fmt)
target_link_libraries(Dependencies INTERFACE fmt)

# ========== RARECPP ========== #
set(RARECPP_DIR ${PROJECT_SOURCE_DIR}/Dependencies/RareCpp/include/rarecpp)
set(RARECPP_SOURCES ${RARECPP_DIR}/reflect.h)
target_sources(${PROJECT_NAME} PRIVATE ${RARECPP_SOURCES})
target_include_directories(Dependencies INTERFACE ${RARECPP_DIR})

# ========== INFOWARE ========== #
if (WIN32)
    set(INFOWARE_USE_D3D 1)
    set(INFOWARE_USE_OPENCL 0)
    set(INFOWARE_USE_OPENGL 0)
else()
    set(INFOWARE_USE_OPENCL 1)
    set(INFOWARE_USE_D3D 0)
    set(INFOWARE_USE_OPENGL 0)
endif()

set(INFOWARE_PCI_IDS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Infoware/pcids/pci.ids)

add_subdirectory(Infoware)
target_link_libraries(Dependencies INTERFACE infoware)

# ========== WHEREAMI ========== #
add_library(WHEREAMI STATIC)
set(WHEREAMI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/whereami/src)
target_sources(WHEREAMI PRIVATE ${WHEREAMI_DIR}/whereami.c ${WHEREAMI_DIR}/whereami.h)
target_include_directories(WHEREAMI PRIVATE ${WHEREAMI_DIR})
target_link_libraries(Dependencies INTERFACE WHEREAMI)
target_include_directories(Dependencies INTERFACE ${WHEREAMI_DIR})

# ========== TBB ========== #
set(TBB_TEST OFF)
set(TBBMALLOC_BUILD OFF)
set(TBBMALLOC_PROXY_BUILD OFF)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(TBB_STRICT ON)
else()
    set(TBB_STRICT OFF)
endif()

find_package(TBB REQUIRED)
target_link_libraries(Dependencies INTERFACE TBB::tbb)
target_compile_options(Dependencies PRIVATE ${TBB_CXX_STD_FLAG})