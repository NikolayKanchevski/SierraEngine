# === PROJECT & CMAKE SETTINGS === #
set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.20)
project(SierraEngine)

# === EXECUTABLE SETTINGS === #
set(SOURCE_FILES Source/main.cpp Source/Core/Rendering/Window.cpp Source/Core/Rendering/Window.h Source/Core/Debugger.cpp Source/Core/Debugger.h Source/Core/Application/Application.cpp Source/Core/Application/Application.h Source/Engine/Classes/Time.cpp Source/Engine/Classes/Time.h Source/Engine/Classes/Cursor.cpp Source/Engine/Classes/Cursor.h Source/Engine/Classes/Input.cpp Source/Engine/Classes/Input.h Source/Core/Rendering/Abstractions/Buffer.cpp Source/Core/Rendering/Abstractions/Buffer.h Source/Core/Rendering/Abstractions/Descriptors.cpp Source/Core/Rendering/Abstractions/Descriptors.h Source/Core/Rendering/Abstractions/Framebuffer.cpp Source/Core/Rendering/Abstractions/Framebuffer.h Source/Core/Rendering/Abstractions/Image.cpp Source/Core/Rendering/Abstractions/Image.h Source/Core/Rendering/Abstractions/Sampler.cpp Source/Core/Rendering/Abstractions/Sampler.h Source/Core/Rendering/Abstractions/Texture.cpp Source/Core/Rendering/Abstractions/Texture.h Source/Engine/Classes/Math.h Source/Engine/Classes/File.cpp Source/Engine/Classes/File.h Source/Engine/Components/DirectionalLight.h Source/Engine/Components/PointLight.h Source/Core/World.cpp Source/Core/World.h Source/Engine/Components/Camera.cpp Source/Engine/Components/Camera.h Source/Engine/Classes/Mesh.cpp Source/Engine/Classes/Mesh.h Source/Engine/Classes/Entity.cpp Source/Engine/Classes/Entity.h Source/Engine/Components/Component.h Source/Engine/Classes/Binary.cpp Source/Engine/Classes/Binary.h Source/Engine/Components/Light.h Source/Engine/Components/MeshRenderer.cpp Source/Engine/Components/MeshRenderer.h Source/Core/Rendering/Abstractions/Device.cpp Source/Core/Rendering/Abstractions/Device.h Source/Core/Rendering/Abstractions/RenderPass.cpp Source/Core/Rendering/Abstractions/RenderPass.h Source/Engine/Include.h Source/Core/Rendering/Types.h Source/Core/Rendering/Abstractions/Shader.cpp Source/Core/Rendering/Abstractions/Shader.h Source/Core/Rendering/Bases/VK.cpp Source/Core/Rendering/Bases/VK.h Source/Core/Rendering/Abstractions/Swapchain.cpp Source/Core/Rendering/Abstractions/Swapchain.h Source/Core/Rendering/Abstractions/CommandBuffer.cpp Source/Core/Rendering/Abstractions/CommandBuffer.h Source/Core/Rendering/Renderers/VulkanRenderer.cpp Source/Core/Rendering/Renderers/VulkanRenderer.h Source/Core/Rendering/UI/ImGuiInstance.cpp Source/Core/Rendering/UI/ImGuiInstance.h Source/Core/Rendering/Abstractions/Queries.cpp Source/Core/Rendering/Abstractions/Queries.h Source/Engine/Classes/RNG.cpp Source/Engine/Classes/RNG.h Source/Core/Rendering/Abstractions/Cubemap.cpp Source/Core/Rendering/Abstractions/Cubemap.h Source/Engine/Components/Transform.cpp Source/Engine/Components/Transform.h Source/Engine/Components/UUID.cpp Source/Engine/Components/UUID.h Source/Engine/Components/Tag.cpp Source/Engine/Components/Tag.h Source/Engine/Components/Relationship.cpp Source/Engine/Components/Relationship.h Source/Core/Internal/Types.h Source/Core/Rendering/Renderers/DeferredVulkanRenderer.cpp Source/Core/Rendering/Renderers/DeferredVulkanRenderer.h Source/Core/Internal/Macros.h Source/Core/Rendering/RenderingSettings.h Source/Engine/Components/ComponentsInclude.h Source/Engine/Classes/ClassesInclude.h Source/Engine/Classes/SystemInformation.cpp Source/Engine/Classes/SystemInformation.h Source/Core/Internal/Definitions.h Source/Core/Rendering/Renderers/ExperimentalVulkanRenderer.cpp Source/Core/Rendering/Renderers/ExperimentalVulkanRenderer.h Source/Core/Rendering/Abstractions/DynamicRenderer.cpp Source/Core/Rendering/Abstractions/DynamicRenderer.h Source/Engine/Classes/IdentifierPool.h Source/Engine/Classes/MemoryObject.h Source/Core/Rendering/Abstractions/Pipeline.cpp Source/Core/Rendering/Abstractions/Pipeline.h Source/Core/Rendering/Abstractions/GraphicsPipeline.cpp Source/Core/Rendering/Abstractions/GraphicsPipeline.h Source/Engine/Classes/MemoryObject.cpp Source/Core/Rendering/Abstractions/ComputePipeline.cpp Source/Core/Rendering/Abstractions/ComputePipeline.h Source/Core/Rendering/Modules/Raycaster.cpp Source/Core/Rendering/Modules/Raycaster.h Source/Engine/Components/DirectionalLight.cpp Source/Engine/Components/PointLight.cpp Source/Engine/Classes/Raycast.h Source/Engine/Classes/Raycast.cpp Source/Engine/Classes/Discord.h Source/Engine/Classes/Discord.cpp Source/Editor/Editor.h Source/Editor/Editor.cpp Source/Engine/Components/Light.cpp Source/Engine/Classes/Material.h Source/Editor/GUI.h Source/Editor/GUI.cpp Source/Editor/GUITypes.h Source/Engine/Handlers/Assets/ArenaAllocator.cpp Source/Engine/Handlers/Assets/ArenaAllocator.h Source/Engine/Classes/Vertex.h Source/Engine/Handlers/Assets/AssetManager.cpp Source/Engine/Handlers/Assets/AssetManager.h Source/Engine/Classes/Asset.h Source/Engine/Handlers/Project.h Source/Engine/Handlers/Project.cpp Source/Core/Internal/Version.h Source/Editor/Panels/MainViewportPanel.cpp Source/Editor/Panels/MainViewportPanel.h Source/Editor/Panels/RendererViewportPanel.h Source/Editor/Panels/RendererViewportPanel.cpp Source/Editor/Panels/HierarchyPanel.cpp Source/Editor/Panels/HierarchyPanel.h Source/Editor/Panels/PropertiesPanel.cpp Source/Editor/Panels/PropertiesPanel.h Source/Editor/Panels/DebugPanel.cpp Source/Editor/Panels/DebugPanel.h Source/Editor/Panels/DiscordDebugPanel.cpp Source/Editor/Panels/DiscordDebugPanel.h Source/Editor/Panels/AssetBrowserPanel.cpp Source/Editor/Panels/AssetBrowserPanel.h Source/Engine/Classes/Model.cpp Source/Engine/Classes/Model.h Source/Engine/Classes/Material.cpp Source/Engine/Handlers/Assets/ResourceSystem.h Source/Engine/Handlers/Assets/ResourceSystem.cpp Source/Engine/Types.h Source/Engine/Classes/Asset.cpp Source/Engine/Handlers/Assets/AssetImporter.cpp Source/Engine/Handlers/Assets/AssetImporter.h)

include(Dependencies/AddIconToBinary.cmake)
AddIconToBinary(SOURCE_FILES ICONS ${CMAKE_SOURCE_DIR}/Media/SierraExecutableIcon.ico ${CMAKE_SOURCE_DIR}/Media/SierraExecutableIcon.icns)
if (MSVC)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
elseif(APPLE)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()

# === COMPILER SETTINGS === #
LIST(APPEND CMAKE_PROGRAM_PATH \"${CMAKE_BINARY_DIR}/DLLs/\")
if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.12.25835)
    set(CMAKE_CXX20_STANDARD_COMPILE_OPTION "-std:c++latest")
    set(CMAKE_CXX20_EXTENSION_COMPILE_OPTION "-std:c++latest")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DDEBUG)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

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
target_precompile_headers(${PROJECT_NAME} PUBLIC Source/Core/Internal/PrecompiledHeaders.h)

# === UPDATE SCRIPT RUNNING === #
set(CURRENT_PROJECT_PATH "../Example Project/")
if(PYTHON_EXECUTABLE)
    if(NOT DEFINED CURRENT_PROJECT_PATH OR CURRENT_PROJECT_PATH STREQUAL "")
        message(WARNING "Cannot automatically update project, as the variable [CURRENT_PROJECT_PATH] has not been se in CMake")
    else()
        add_custom_target(RunUpdatePythonScript
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/Scripts/UpdateProject.py ${CURRENT_PROJECT_PATH}
        )
        add_dependencies(${PROJECT_NAME} RunUpdatePythonScript)
    endif()
else()
    message(WARNING "Python install was not found on the machine. Automatic update scripts cannot be run!")
endif()