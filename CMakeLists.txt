cmake_minimum_required(VERSION 3.20)
project(SierraEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp Core/Rendering/Window.cpp Core/Rendering/Window.h Engine/Classes/Stopwatch.cpp Engine/Classes/Stopwatch.h Core/Debugger.cpp Core/Debugger.h Core/Application/Application.cpp Core/Application/Application.h Engine/Classes/Time.cpp Engine/Classes/Time.h Engine/Classes/Cursor.cpp Engine/Classes/Cursor.h Core/Rendering/Vulkan/VulkanCore.h Engine/Classes/Input.cpp Engine/Classes/Input.h Core/Rendering/Vulkan/Abstractions/Buffer.cpp Core/Rendering/Vulkan/Abstractions/Buffer.h Core/Rendering/Vulkan/Abstractions/Descriptors.cpp Core/Rendering/Vulkan/Abstractions/Descriptors.h Core/Rendering/Vulkan/Abstractions/Framebuffer.cpp Core/Rendering/Vulkan/Abstractions/Framebuffer.h Core/Rendering/Vulkan/Abstractions/Image.cpp Core/Rendering/Vulkan/Abstractions/Image.h Core/Rendering/Vulkan/Abstractions/Sampler.cpp Core/Rendering/Vulkan/Abstractions/Sampler.h Core/Rendering/Vulkan/Abstractions/Texture.cpp Core/Rendering/Vulkan/Abstractions/Texture.h Core/Rendering/Vulkan/VulkanUtilities.cpp Core/Rendering/Vulkan/VulkanUtilities.h Engine/Classes/Math.cpp Engine/Classes/Math.h Engine/Classes/File.cpp Engine/Classes/File.h Core/Rendering/Vulkan/VulkanCore.cpp Core/Version.h Core/Rendering/Vulkan/Renderer/VulkanRenderer.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer.h Core/Rendering/Vulkan/Renderer/VulkanRenderer_Swapchain.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Commands.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_RenderPass.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_PushConstants.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Descriptors.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_GraphicsPipeline.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Framebuffers.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Textures.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Buffers.cpp Engine/Components/Lighting/DirectionalLight.h Engine/Components/Lighting/PointLight.h Core/World.cpp Core/World.h Core/Rendering/Vulkan/Renderer/VulkanRenderer_Queries.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_Drawing.cpp Core/Rendering/Vulkan/Renderer/VulkanRenderer_ImGui.cpp Engine/Components/Camera.cpp Engine/Components/Camera.h Engine/Classes/Mesh.cpp Engine/Classes/Mesh.h Engine/Components/Model.cpp Engine/Components/Model.h Engine/Structures/Vertex.h Engine/Structures/Material.h Engine/Classes/Entity.cpp Engine/Classes/Entity.h Engine/Components/InternalComponents.h Engine/Components/Component.h Core/Application/Application_UI.cpp Engine/Classes/Binary.cpp Engine/Classes/Binary.h Engine/Components/Lighting/Light.h Engine/Components/MeshRenderer.cpp Engine/Components/MeshRenderer.h Core/Rendering/Vulkan/Renderer/VulkanRenderer_Offscreen.cpp Core/Rendering/Vulkan/Abstractions/OffscreenRenderer.cpp Core/Rendering/Vulkan/Abstractions/OffscreenRenderer.h Core/Rendering/Vulkan/Abstractions/Device.cpp Core/Rendering/Vulkan/Abstractions/Device.h Core/Rendering/Vulkan/Renderer/VulkanRenderer_Device.cpp Core/Rendering/Vulkan/Abstractions/RenderPass.cpp Core/Rendering/Vulkan/Abstractions/RenderPass.h Engine/Include.h Core/Rendering/Vulkan/VulkanTypes.h)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# vcpkg install vulkan
find_package(Vulkan REQUIRED)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc REQUIRED)

# vcpkg install glm
find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glm::glm)

# vcpkg install glfw3
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)

# vcpkg install "imgui[core,docking-experimental,glfw-binding,vulkan-binding]"
find_package(imgui CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

# vcpkg install assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# vcpkg install stb
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})

# vcpkg install entt
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)