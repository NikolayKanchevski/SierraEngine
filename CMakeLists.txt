cmake_minimum_required(VERSION 3.22)
project(SierraEngine)

set(CMAKE_CXX_STANDARD 17)

add_executable(${PROJECT_NAME} main.cpp Core/Rendering/Window.cpp Core/Rendering/Window.h Engine/Classes/Stopwatch.cpp Engine/Classes/Stopwatch.h Core/Rendering/Vulkan/VulkanDebugger.cpp Core/Rendering/Vulkan/VulkanDebugger.h Core/Application/Application.cpp Core/Application/Application.h)

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



