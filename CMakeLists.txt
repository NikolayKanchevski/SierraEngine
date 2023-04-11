# === PROJECT & CMAKE SETTINGS === #
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.20)
project(SierraEngine)

set(SOURCE_FILES main.cpp Core/Rendering/Window.cpp Core/Rendering/Window.h Engine/Classes/Stopwatch.cpp Engine/Classes/Stopwatch.h Core/Debugger.cpp Core/Debugger.h Core/Application/Application.cpp Core/Application/Application.h Engine/Classes/Time.cpp Engine/Classes/Time.h Engine/Classes/Cursor.cpp Engine/Classes/Cursor.h Engine/Classes/Input.cpp Engine/Classes/Input.h Core/Rendering/Vulkan/Abstractions/Buffer.cpp Core/Rendering/Vulkan/Abstractions/Buffer.h Core/Rendering/Vulkan/Abstractions/Descriptors.cpp Core/Rendering/Vulkan/Abstractions/Descriptors.h Core/Rendering/Vulkan/Abstractions/Framebuffer.cpp Core/Rendering/Vulkan/Abstractions/Framebuffer.h Core/Rendering/Vulkan/Abstractions/Image.cpp Core/Rendering/Vulkan/Abstractions/Image.h Core/Rendering/Vulkan/Abstractions/Sampler.cpp Core/Rendering/Vulkan/Abstractions/Sampler.h Core/Rendering/Vulkan/Abstractions/Texture.cpp Core/Rendering/Vulkan/Abstractions/Texture.h Engine/Classes/Math.h Engine/Classes/File.cpp Engine/Classes/File.h Core/Version.h Engine/Components/Lighting/DirectionalLight.h Engine/Components/Lighting/PointLight.h Core/World.cpp Core/World.h Engine/Components/Camera.cpp Engine/Components/Camera.h Engine/Classes/Mesh.cpp Engine/Classes/Mesh.h Engine/Components/Model.cpp Engine/Components/Model.h Engine/Classes/Entity.cpp Engine/Classes/Entity.h Engine/Components/Component.h Engine/Classes/Binary.cpp Engine/Classes/Binary.h Engine/Components/Lighting/Light.h Engine/Components/MeshRenderer.cpp Engine/Components/MeshRenderer.h Core/Rendering/Vulkan/Abstractions/Device.cpp Core/Rendering/Vulkan/Abstractions/Device.h Core/Rendering/Vulkan/Abstractions/RenderPass.cpp Core/Rendering/Vulkan/Abstractions/RenderPass.h Engine/Include.h Core/Rendering/Vulkan/VulkanTypes.h Core/Rendering/UI/ImGuiCore.cpp Core/Rendering/UI/ImGuiCore.h Core/Rendering/Vulkan/Abstractions/Shader.cpp Core/Rendering/Vulkan/Abstractions/Shader.h Core/Rendering/Vulkan/Abstractions/Pipelines.h Core/Rendering/Vulkan/VK.cpp Core/Rendering/Vulkan/VK.h Core/Rendering/Vulkan/Abstractions/Swapchain.cpp Core/Rendering/Vulkan/Abstractions/Swapchain.h Core/EngineCore.cpp Core/EngineCore.h Core/Rendering/Vulkan/Abstractions/CommandBuffer.cpp Core/Rendering/Vulkan/Abstractions/CommandBuffer.h Core/Rendering/Vulkan/Renderers/VulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/VulkanRenderer.h Core/Rendering/UI/ImGuiInstance.cpp Core/Rendering/UI/ImGuiInstance.h Core/Rendering/Vulkan/Abstractions/Queries.cpp Core/Rendering/Vulkan/Abstractions/Queries.h Engine/Classes/RNG.cpp Engine/Classes/RNG.h Core/Rendering/Vulkan/Abstractions/Cubemap.cpp Core/Rendering/Vulkan/Abstractions/Cubemap.h Core/Rendering/Math/MatrixUtilities.h Core/Rendering/UI/ImGuiUtilities.h Core/Rendering/UI/ImGuiUtilities.cpp Engine/Components/Transform.cpp Engine/Components/Transform.h Engine/Components/UUID.cpp Engine/Components/UUID.h Engine/Components/Tag.cpp Engine/Components/Tag.h Engine/Components/Relationship.cpp Engine/Components/Relationship.h Core/Internal/Types.h Core/Rendering/Vulkan/Renderers/DeferredVulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/DeferredVulkanRenderer.h Core/Internal/Macros.h Core/Rendering/RenderingTemplates.h Core/Rendering/RenderingSettings.h Core/Rendering/Vulkan/Abstractions/Abstractions.h Engine/Components/Components.h Engine/Classes/Classes.h Engine/Classes/SystemInformation.cpp Engine/Classes/SystemInformation.h Core/Internal/Definitions.h Core/Rendering/UI/Panels/UIPanels.h Core/Rendering/UI/Panels/UIPanels.cpp Core/Rendering/Vulkan/Abstractions/Pipelines.h Core/Rendering/Vulkan/RenderingUtilities.cpp Core/Rendering/Vulkan/RenderingUtilities.h Core/Rendering/Vulkan/Renderers/ExperimentalVulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/ExperimentalVulkanRenderer.h Core/Rendering/Vulkan/Abstractions/DynamicRenderer.cpp Core/Rendering/Vulkan/Abstractions/DynamicRenderer.h Engine/Classes/IdentifierPool.h Core/Rendering/Math/MatrixUtilities.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# === COMPILER SETTINGS === #
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# === DEPENDENCIES LINKAGE === #
add_subdirectory(Dependencies)
target_link_libraries(${PROJECT_NAME} PRIVATE Dependencies)

# TODO: Add as git submodule or replace entirely
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# === PRECOMPILED HEADERS LINKAGE === #
target_precompile_headers(${PROJECT_NAME} PUBLIC Core/Internal/PrecompiledHeaders.h)