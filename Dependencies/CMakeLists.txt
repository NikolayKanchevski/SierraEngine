set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.20)

include(ExternalProject)
add_library(Dependencies INTERFACE)

# ========== VULKAN ========== #
if(NOT DEFINED "VULKAN_SDK")
    message(FATAL_ERROR "Did you forget to set the [VULKAN_SDK] CMake variable?")
endif()

find_package(Vulkan REQUIRED)
target_include_directories(Dependencies INTERFACE "${VULKAN_SDK}/include")
target_link_libraries(Dependencies INTERFACE Vulkan::Vulkan)

# ========== KHRONOS UTILITIES ========== #
add_subdirectory(glslang)
add_subdirectory(SPIRV-Headers)
add_subdirectory(SPIRV-Tools)

# ========== VMA ========== #
add_subdirectory(VMA)
target_include_directories(Dependencies INTERFACE VMA/include)

# ========== SHADERC ========== #
set(SHADERC_SKIP_TESTS ON)
set(SHADERC_SKIP_EXAMPLES ON)
set(SHADERC_ENABLE_TESTS OFF)

add_subdirectory(Shaderc)
target_link_libraries(Dependencies INTERFACE shaderc)

# ========== GLFW ========== #
set(GLFW_BUILD_EXAMPLES, OFF)
set(GLFW_BUILD_TESTS, OFF)
set(GLFW_BUILD_DOCS, OFF)

add_subdirectory(GLFW)
target_link_libraries(Dependencies INTERFACE glfw)
target_include_directories(Dependencies INTERFACE GLFW/include)

# ========== GLM ========== #
add_subdirectory(glm)
target_link_libraries(Dependencies INTERFACE glm)
target_include_directories(Dependencies INTERFACE glm)

# ========== IMGUI ========== #
add_library(IMGUI STATIC)
target_sources(IMGUI PRIVATE ImGui/imgui_demo.cpp ImGui/imgui_draw.cpp ImGui/imgui_tables.cpp ImGui/imgui_widgets.cpp ImGui/imgui.cpp ImGui/imgui.h ImGui/imgui_internal.h ImGui/imconfig.h ImGui/imstb_rectpack.h ImGui/imstb_textedit.h ImGui/imstb_truetype.h ImGui/backends/imgui_impl_vulkan.cpp ImGui/backends/imgui_impl_vulkan.h ImGui/backends/imgui_impl_glfw.cpp ImGui/backends/imgui_impl_glfw.h)
target_include_directories(IMGUI PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/GLFW/include/ ${VULKAN_SDK}/include ImGui/backends ImGui)
target_link_libraries(Dependencies INTERFACE IMGUI)
target_include_directories(Dependencies INTERFACE ImGui ImGui/backends)

# ========== IMGUIZMO ========== #
add_library(IMGUIZMO STATIC)
target_sources(IMGUIZMO PRIVATE ImGuizmo/ImGuizmo.cpp ImGuizmo/ImGuizmo.h)
target_include_directories(IMGUIZMO PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ImGui/)
target_link_libraries(Dependencies INTERFACE IMGUIZMO)
target_include_directories(Dependencies INTERFACE ImGuizmo)

# ========== ENTT ========== #
add_subdirectory(EnTT)
target_link_libraries(Dependencies INTERFACE EnTT)
target_include_directories(Dependencies INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/EnTT/single_include)

# ========== STB ========== #
target_include_directories(Dependencies INTERFACE stb)

# ========== FMT ========== #
set(FMT_DOC OFF)
set(FMT_TEST OFF)

add_subdirectory(fmt)
target_link_libraries(Dependencies INTERFACE fmt)

# ========== RARECPP ========== #
set(RARECPP_DIR "${PROJECT_SOURCE_DIR}/Dependencies/RareCpp/RareCppLib")
set(RARECPP_SOURCES "${RARECPP_DIR}/Reflect.h")
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

set(INFOWARE_PCI_IDS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Infoware/pcids/pci.ids")

add_subdirectory(Infoware)
target_link_libraries(Dependencies INTERFACE infoware)

# ========== WHEREAMI ========== #
add_library(WHEREAMI STATIC)
set(WHEREAMI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/whereami/src")
target_sources(WHEREAMI PRIVATE "${WHEREAMI_DIR}/whereami.c" "${WHEREAMI_DIR}/whereami.h")
target_include_directories(WHEREAMI PRIVATE ${WHEREAMI_DIR})
target_link_libraries(Dependencies INTERFACE WHEREAMI)
target_include_directories(Dependencies INTERFACE ${WHEREAMI_DIR})

# ========== TBB ========== #
ExternalProject_Add(TBB-project
    GIT_REPOSITORY https://github.com/oneapi-src/oneTBB
    GIT_TAG 154cc73ca4d359621202399cc0c3c91058e56e79
    GIT_SUBMODULES_RECURSE ON
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
    INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/install"
    LIST_SEPARATOR |
    CMAKE_CACHE_ARGS
    "-DCMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}:STRING=${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}"
    "-DCMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}:STRING=${CMAKE_C_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}"
    "-DCMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UPPER}:STRING=${CMAKE_EXE_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}"
    "-DCMAKE_SHARED_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UPPER}:STRING=${CMAKE_SHARED_LINKER_FLAGS_${CMAKE_BUILD_TYPE_UPPER}}"
    "-DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_INSTALL_PREFIX:STRING=<INSTALL_DIR>"
)

add_library(TBB-lib INTERFACE)
add_dependencies(TBB-lib TBB-project)
ExternalProject_Get_property(TBB-project INSTALL_DIR)
target_include_directories(TBB-lib SYSTEM INTERFACE "${INSTALL_DIR}/include")
target_link_directories(TBB-lib INTERFACE "${INSTALL_DIR}/lib")

find_package(TBB)
target_link_libraries(TBB-lib INTERFACE TBB::tbb)

target_link_libraries(Dependencies INTERFACE TBB-lib)