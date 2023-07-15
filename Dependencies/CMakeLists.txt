set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.20)

include(Macros.cmake)
include(DeviceInfo.cmake)
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
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)
endif()

add_subdirectory(Assimp)
target_link_libraries(Dependencies INTERFACE assimp)
target_include_directories(Dependencies SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Assimp/include/)

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
if(WIN32)
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
set(BUILD_SHARED_LIBS OFF)
set(TBBMALLOC_PROXY_BUILD OFF)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(TBB_STRICT ON)
else()
    set(TBB_STRICT OFF)
endif()

add_subdirectory(TBB)
target_link_libraries(Dependencies INTERFACE TBB::tbb)
target_compile_options(Dependencies PRIVATE ${TBB_CXX_STD_FLAG})

# ========== DISCORD SDK ========== #
add_library(DiscordSDK STATIC)
file(GLOB_RECURSE DISCORD_SDK_HEADER_FILES DiscordSDK/src/*.h)
file(GLOB_RECURSE DISCORD_SDK_SOURCE_FILES DiscordSDK/src/*.cpp)
target_sources(DiscordSDK PRIVATE ${DISCORD_SDK_HEADER_FILES} ${DISCORD_SDK_SOURCE_FILES})

target_link_libraries(Dependencies INTERFACE DiscordSDK)
target_include_directories(Dependencies INTERFACE DiscordSDK/src/)
set(DISCORD_SDK_LIB_PATH DiscordSDK/lib)
if(DEVICE_OS STREQUAL "Windows")
    if (DEVICE_ARCHITECTURE STREQUAL "x64")
        LINK_DYNAMIC_LIBRARY(Dependencies INTERFACE ${DISCORD_SDK_LIB_PATH}/x86_64/discord_game_sdk.dll)
    endif()

elseif(DEVICE_OS STREQUAL "macOS" OR DEVICE_OS STREQUAL "iOS")
    if (DEVICE_ARCHITECTURE STREQUAL "arm64")
        LINK_DYNAMIC_LIBRARY(Dependencies INTERFACE ${DISCORD_SDK_LIB_PATH}/aarch64/discord_game_sdk.dylib)
    endif()

elseif(DEVICE_OS STREQUAL "Linux" OR DEVICE_OS STREQUAL "Android")
    if (DEVICE_ARCHITECTURE STREQUAL "x64")
        LINK_DYNAMIC_LIBRARY(Dependencies INTERFACE ${DISCORD_SDK_LIB_PATH}/x86_64/discord_game_sdk.so)
    endif()
else()
    message(WARNING "Cannot link Discord SDK on this system")
endif()