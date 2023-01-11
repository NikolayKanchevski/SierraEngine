cmake_minimum_required(VERSION 3.20)
project(SierraEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(${PROJECT_NAME} main.cpp Core/Rendering/Window.cpp Core/Rendering/Window.h Engine/Classes/Stopwatch.cpp Engine/Classes/Stopwatch.h Core/Debugger.cpp Core/Debugger.h Core/Application/Application.cpp Core/Application/Application.h Engine/Classes/Time.cpp Engine/Classes/Time.h Engine/Classes/Cursor.cpp Engine/Classes/Cursor.h Engine/Classes/Input.cpp Engine/Classes/Input.h Core/Rendering/Vulkan/Abstractions/Buffer.cpp Core/Rendering/Vulkan/Abstractions/Buffer.h Core/Rendering/Vulkan/Abstractions/Descriptors.cpp Core/Rendering/Vulkan/Abstractions/Descriptors.h Core/Rendering/Vulkan/Abstractions/Framebuffer.cpp Core/Rendering/Vulkan/Abstractions/Framebuffer.h Core/Rendering/Vulkan/Abstractions/Image.cpp Core/Rendering/Vulkan/Abstractions/Image.h Core/Rendering/Vulkan/Abstractions/Sampler.cpp Core/Rendering/Vulkan/Abstractions/Sampler.h Core/Rendering/Vulkan/Abstractions/Texture.cpp Core/Rendering/Vulkan/Abstractions/Texture.h Engine/Classes/Math.cpp Engine/Classes/Math.h Engine/Classes/File.cpp Engine/Classes/File.h Core/Version.h Engine/Components/Lighting/DirectionalLight.h Engine/Components/Lighting/PointLight.h Core/World.cpp Core/World.h Engine/Components/Camera.cpp Engine/Components/Camera.h Engine/Classes/Mesh.cpp Engine/Classes/Mesh.h Engine/Components/Model.cpp Engine/Components/Model.h Engine/Structures/Vertex.h Engine/Structures/Material.h Engine/Classes/Entity.cpp Engine/Classes/Entity.h Engine/Components/InternalComponents.h Engine/Components/Component.h Core/Application/Application_UI.cpp Engine/Classes/Binary.cpp Engine/Classes/Binary.h Engine/Components/Lighting/Light.h Engine/Components/MeshRenderer.cpp Engine/Components/MeshRenderer.h Core/Rendering/Vulkan/Abstractions/OffscreenRenderer.cpp Core/Rendering/Vulkan/Abstractions/OffscreenRenderer.h Core/Rendering/Vulkan/Abstractions/Device.cpp Core/Rendering/Vulkan/Abstractions/Device.h Core/Rendering/Vulkan/Abstractions/RenderPass.cpp Core/Rendering/Vulkan/Abstractions/RenderPass.h Engine/Include.h Core/Rendering/Vulkan/VulkanTypes.h Core/Rendering/UI/ImGuiCore.cpp Core/Rendering/UI/ImGuiCore.h Core/Rendering/Vulkan/Abstractions/Shader.cpp Core/Rendering/Vulkan/Abstractions/Shader.h Core/Rendering/Vulkan/Abstractions/Pipeline.h Engine/Components/InternalComponents.cpp Core/Rendering/Vulkan/VK.cpp Core/Rendering/Vulkan/VK.h Core/Rendering/Vulkan/Abstractions/Swapchain.cpp Core/Rendering/Vulkan/Abstractions/Swapchain.h Core/EngineCore.cpp Core/EngineCore.h Core/Rendering/Vulkan/Abstractions/CommandBuffer.cpp Core/Rendering/Vulkan/Abstractions/CommandBuffer.h Core/Rendering/Vulkan/Abstractions/VulkanRenderer.cpp Core/Rendering/Vulkan/Abstractions/VulkanRenderer.h Core/Rendering/UI/ImGuiInstance.cpp Core/Rendering/UI/ImGuiInstance.h Core/Rendering/Vulkan/Renderers/MainVulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/MainVulkanRenderer.h Core/Rendering/Vulkan/Abstractions/Queries.cpp Core/Rendering/Vulkan/Abstractions/Queries.h Engine/Classes/RNG.cpp Engine/Classes/RNG.h Core/Rendering/Vulkan/Abstractions/Cubemap.cpp Core/Rendering/Vulkan/Abstractions/Cubemap.h Core/Rendering/Math/MatrixUtilities.h)

if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
endif()

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

# --- IMGUIZMO
set(IMGUIZMO_DIR "${PROJECT_SOURCE_DIR}/Dependencies/ImGuizmo/include")
set(IMGUIZMO_SOURCES "${IMGUIZMO_DIR}/ImGuizmo.cpp" "${IMGUIZMO_DIR}/ImGuizmo.h")
target_sources(${PROJECT_NAME} PRIVATE ${IMGUIZMO_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${IMGUIZMO_DIR})

# vcpkg install assimp
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# vcpkg install stb
find_path(STB_INCLUDE_DIRS "stb_c_lexer.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})

# vcpkg install entt
find_package(EnTT CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE EnTT::EnTT)

# vcpkg install tbb
find_package(TBB CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb TBB::tbbmalloc)

# vcpkg install vulkan-memory-allocator
find_package(unofficial-vulkan-memory-allocator CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::vulkan-memory-allocator::vulkan-memory-allocator)