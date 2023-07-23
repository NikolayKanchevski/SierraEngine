# === PROJECT & CMAKE SETTINGS === #
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.20)
project(SierraEngine)

set(SOURCE_FILES main.cpp Core/Rendering/Window.cpp Core/Rendering/Window.h Engine/Classes/Stopwatch.cpp Engine/Classes/Stopwatch.h Core/Debugger.cpp Core/Debugger.h Core/Application/Application.cpp Core/Application/Application.h Engine/Classes/Time.cpp Engine/Classes/Time.h Engine/Classes/Cursor.cpp Engine/Classes/Cursor.h Engine/Classes/Input.cpp Engine/Classes/Input.h Core/Rendering/Vulkan/Abstractions/Buffer.cpp Core/Rendering/Vulkan/Abstractions/Buffer.h Core/Rendering/Vulkan/Abstractions/Descriptors.cpp Core/Rendering/Vulkan/Abstractions/Descriptors.h Core/Rendering/Vulkan/Abstractions/Framebuffer.cpp Core/Rendering/Vulkan/Abstractions/Framebuffer.h Core/Rendering/Vulkan/Abstractions/Image.cpp Core/Rendering/Vulkan/Abstractions/Image.h Core/Rendering/Vulkan/Abstractions/Sampler.cpp Core/Rendering/Vulkan/Abstractions/Sampler.h Core/Rendering/Vulkan/Abstractions/Texture.cpp Core/Rendering/Vulkan/Abstractions/Texture.h Engine/Classes/Math.h Engine/Classes/File.cpp Engine/Classes/File.h Engine/Components/Lighting/DirectionalLight.h Engine/Components/Lighting/PointLight.h Core/World.cpp Core/World.h Engine/Components/Camera.cpp Engine/Components/Camera.h Engine/Classes/Mesh.cpp Engine/Classes/Mesh.h Engine/Components/Model.cpp Engine/Components/Model.h Engine/Classes/Entity.cpp Engine/Classes/Entity.h Engine/Components/Component.h Engine/Classes/Binary.cpp Engine/Classes/Binary.h Engine/Components/Lighting/Light.h Engine/Components/MeshRenderer.cpp Engine/Components/MeshRenderer.h Core/Rendering/Vulkan/Abstractions/Device.cpp Core/Rendering/Vulkan/Abstractions/Device.h Core/Rendering/Vulkan/Abstractions/RenderPass.cpp Core/Rendering/Vulkan/Abstractions/RenderPass.h Engine/Include.h Core/Rendering/Vulkan/VulkanTypes.h Core/Rendering/Vulkan/Abstractions/Shader.cpp Core/Rendering/Vulkan/Abstractions/Shader.h Core/Rendering/Vulkan/VK.cpp Core/Rendering/Vulkan/VK.h Core/Rendering/Vulkan/Abstractions/Swapchain.cpp Core/Rendering/Vulkan/Abstractions/Swapchain.h Core/Rendering/Vulkan/Abstractions/CommandBuffer.cpp Core/Rendering/Vulkan/Abstractions/CommandBuffer.h Core/Rendering/Vulkan/Renderers/VulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/VulkanRenderer.h Core/Rendering/UI/ImGuiInstance.cpp Core/Rendering/UI/ImGuiInstance.h Core/Rendering/Vulkan/Abstractions/Queries.cpp Core/Rendering/Vulkan/Abstractions/Queries.h Engine/Classes/RNG.cpp Engine/Classes/RNG.h Core/Rendering/Vulkan/Abstractions/Cubemap.cpp Core/Rendering/Vulkan/Abstractions/Cubemap.h Engine/Components/Transform.cpp Engine/Components/Transform.h Engine/Components/UUID.cpp Engine/Components/UUID.h Engine/Components/Tag.cpp Engine/Components/Tag.h Engine/Components/Relationship.cpp Engine/Components/Relationship.h Core/Internal/Types.h Core/Rendering/Vulkan/Renderers/DeferredVulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/DeferredVulkanRenderer.h Core/Internal/Macros.h Core/Rendering/RenderingSettings.h Engine/Components/ComponentsInclude.h Engine/Classes/ClassesInclude.h Engine/Classes/SystemInformation.cpp Engine/Classes/SystemInformation.h Core/Internal/Definitions.h Editor/UIPanels.h Editor/UIPanels.cpp Core/Rendering/Vulkan/Renderers/ExperimentalVulkanRenderer.cpp Core/Rendering/Vulkan/Renderers/ExperimentalVulkanRenderer.h Core/Rendering/Vulkan/Abstractions/DynamicRenderer.cpp Core/Rendering/Vulkan/Abstractions/DynamicRenderer.h Engine/Classes/IdentifierPool.h Engine/Classes/MemoryObject.h Core/Rendering/Vulkan/Abstractions/Pipeline.cpp Core/Rendering/Vulkan/Abstractions/Pipeline.h Core/Rendering/Vulkan/Abstractions/GraphicsPipeline.cpp Core/Rendering/Vulkan/Abstractions/GraphicsPipeline.h Engine/Classes/MemoryObject.cpp Core/Rendering/Vulkan/Abstractions/ComputePipeline.cpp Core/Rendering/Vulkan/Abstractions/ComputePipeline.h Core/Rendering/Modules/Raycaster.cpp Core/Rendering/Modules/Raycaster.h Engine/Components/Lighting/DirectionalLight.cpp Engine/Components/Lighting/PointLight.cpp Engine/Classes/Raycast.h Engine/Classes/Raycast.cpp Engine/Classes/Discord.h Engine/Classes/Discord.cpp Editor/Editor.h Editor/Editor.cpp Engine/Components/Lighting/Light.cpp Engine/Classes/Material.h Editor/GUI.h Editor/GUI.cpp Editor/GUITypes.h Core/Rendering/Modules/ArenaAllocator.cpp Core/Rendering/Modules/ArenaAllocator.h Engine/Classes/Vertex.h)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# === COMPILER SETTINGS === #
LIST(APPEND CMAKE_PROGRAM_PATH \"${CMAKE_BINARY_DIR}/DLLs/\")
message(${CMAKE_PROGRAM_PATH})
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DDEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# === RESOURCE COPY TO OUTPUT FOLDER === #
file(COPY Core/Rendering/Shading/Shaders/ DESTINATION ${CMAKE_BINARY_DIR}/Shaders/)
file(COPY Core/Rendering/Textures/        DESTINATION ${CMAKE_BINARY_DIR}/Textures/)
file(COPY Core/Rendering/Models/          DESTINATION ${CMAKE_BINARY_DIR}/Models/)
file(COPY Core/Rendering/Fonts/           DESTINATION ${CMAKE_BINARY_DIR}/Fonts/)

# === LOCAL SECRETS === #
if(NOT DEFINED DISCORD_APPLICATION_KEY OR DISCORD_APPLICATION_KEY STREQUAL "")
    message(WARNING "Discord application key is not set through CMake options! If you want to take advantage from the Discord integration, make sure to set it.")
    target_compile_definitions(${PROJECT_NAME} PRIVATE SR_DISCORD_APPLICATION_KEY=0)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE SR_DISCORD_APPLICATION_KEY=${DISCORD_APPLICATION_KEY})
endif()

# === DEPENDENCIES LINKAGE === #
add_subdirectory(Dependencies)
target_link_libraries(${PROJECT_NAME} PRIVATE Dependencies)

# === PRECOMPILED HEADERS LINKAGE === #
target_precompile_headers(${PROJECT_NAME} PUBLIC Core/Internal/PrecompiledHeaders.h)

# === UPDATE SCRIPT RUNNING === #
if(PYTHON_EXECUTABLE)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE_ARGUMENT "--Debug")
    else()
        set(CMAKE_BUILD_TYPE_ARGUMENT "--Release")
    endif()

    add_custom_target(RunUpdatePythonScript
        COMMAND ${PYTHON_EXECUTABLE} Scripts/UpdateProject.py ${CMAKE_BUILD_TYPE_ARGUMENT} --CMakeCall
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    add_dependencies(${PROJECT_NAME} RunUpdatePythonScript)
else()
    message(WARNING "Python install was not found on the machine. Automatic update scripts will not be run!")
endif()