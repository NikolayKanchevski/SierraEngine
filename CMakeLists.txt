cmake_minimum_required(VERSION 3.20)
project(SierraEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp Core/Rendering/Window.cpp Core/Rendering/Window.h Engine/Classes/Stopwatch.cpp Engine/Classes/Stopwatch.h Core/Rendering/Vulkan/VulkanDebugger.cpp Core/Rendering/Vulkan/VulkanDebugger.h Core/Application/Application.cpp Core/Application/Application.h Engine/Classes/Time.cpp Engine/Classes/Time.h Engine/Classes/Cursor.cpp Engine/Classes/Cursor.h Core/Rendering/Vulkan/VulkanCore.h Engine/Classes/Input.cpp Engine/Classes/Input.h Core/Rendering/Vulkan/Abstractions/Buffer.cpp Core/Rendering/Vulkan/Abstractions/Buffer.h Core/Rendering/Vulkan/Abstractions/Descriptors.cpp Core/Rendering/Vulkan/Abstractions/Descriptors.h Core/Rendering/Vulkan/Abstractions/Framebuffer.cpp Core/Rendering/Vulkan/Abstractions/Framebuffer.h Core/Rendering/Vulkan/Abstractions/Image.cpp Core/Rendering/Vulkan/Abstractions/Image.h Core/Rendering/Vulkan/Abstractions/RenderPass.cpp Core/Rendering/Vulkan/Abstractions/RenderPass.h Core/Rendering/Vulkan/Abstractions/Sampler.cpp Core/Rendering/Vulkan/Abstractions/Sampler.h Core/Rendering/Vulkan/Abstractions/Texture.cpp Core/Rendering/Vulkan/Abstractions/Texture.h Core/Rendering/Vulkan/VulkanUtilities.cpp Core/Rendering/Vulkan/VulkanUtilities.h Engine/Classes/Math.cpp Engine/Classes/Math.h Engine/Classes/File.cpp Engine/Classes/File.h Core/Rendering/Vulkan/VulkanCore.cpp Engine/Classes/Version.h Core/Rendering/Vulkan/Renderer/VulkanRenderer.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer.h Core/Rendering/Vulkan/Renderer/VulkanRenderer_Instance.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Validation.cpp)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

# vcpkg install vulkan
find_package(Vulkan REQUIRED)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc REQUIRED)

# vcpkg install glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

# vcpkg install glfw3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)

# vcpkg install imgui
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

# vcpkg install assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# vcpkg install stb
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})



