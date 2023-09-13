# === OPTIONS === #
option(SIERRA_BUILD_STATIC_LIBRARY "Wether to build the engine as a static library, that is embedded in the application" OFF)
option(SIERRA_BUILD_SHARED_LIBRARY "Wether to build the engine as a shared library (.dll, .dylib, etc.)" OFF)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(SIERRA_ENABLE_LOGGING "Wether to enable both Application and Engine logging." ON)
else()
    option(SIERRA_ENABLE_LOGGING "Wether to enable both Application and Engine logging." OFF)
endif()
option(SIERRA_BUILD_VULKAN "Wether to build Vulkan resources and link the API" ON)

# === CMAKE CONFIGURATION === #
include(cmake/DeviceInfo.cmake)
cmake_minimum_required(VERSION 3.20)

# === CONFIGURATION VALIDATION === #
if((${SIERRA_BUILD_STATIC_LIBRARY} AND ${SIERRA_BUILD_SHARED_LIBRARY}) OR (NOT ${SIERRA_BUILD_STATIC_LIBRARY} AND NOT ${SIERRA_BUILD_SHARED_LIBRARY}))
    message(FATAL_ERROR "Setting SIERRA_BUILD_STATIC_LIBRARY and SIERRA_BUILD_SHARED_LIBRARY both to ON/OFF for Sierra's CMake project is not allowed! Either one must be ON, and the other has to be turned OFF!")
endif()

# === LIBRARY GENERATION === #
if(${SIERRA_BUILD_STATIC_LIBRARY})
    message(STATUS "[Sierra]: Building Sierra as static library...")
    add_library(Sierra STATIC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE "SIERRA_BUILD_STATIC_LIBRARY")

elseif(${SIERRA_BUILD_SHARED_LIBRARY})
    message(STATUS "[Sierra]: Building Sierra as dynamic library...")
    add_library(Sierra SHARED)
    add_custom_command(TARGET Sierra POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:Sierra> ${CMAKE_BINARY_DIR}/)

    target_compile_definitions(Sierra PRIVATE "SIERRA_USE_SHARED_LIBRARY")
    target_compile_definitions(${PROJECT_NAME} PRIVATE "SIERRA_USE_SHARED_LIBRARY")
    target_compile_definitions(Sierra PRIVATE "SIERRA_BUILD_SHARED_LIBRARY")

else()
    message(FATAL_ERROR "Incorrect configuration for Sierra - either SIERRA_BUILD_STATIC_LIBRARY, or SIERRA_BUILD_SHARED_LIBRARY must be turned on!")

endif()

# === COMPILER SETTINGS === #
set(CMAKE_CXX_STANDARD 20)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(
        -Wno-unknown-warning-option
        -Wno-deprecated-anon-enum-enum-conversion
    )
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()

# === OPTIONS HANDLING === #
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_definitions(Sierra PRIVATE "SR_DEBUG")
    target_compile_definitions(${PROJECT_NAME} PRIVATE "SR_DEBUG")
endif()

if(${SIERRA_ENABLE_LOGGING})
    target_compile_definitions(Sierra PRIVATE "SR_ENABLE_LOGGING")
    target_compile_definitions(${PROJECT_NAME} PRIVATE "SR_ENABLE_LOGGING")
endif()

# === SHARED SOURCE FILE LINKAGE === #
set(INCLUDE_DIRECTORIES src/ include/)
target_sources(Sierra PRIVATE
    include/Sierra.h

    src/Sierra/Core/API.h
    src/Sierra/Application.cpp
    src/Sierra/Application.h
    src/Sierra/Core/Logger.cpp
    src/Sierra/Core/Logger.h
    src/Sierra/Core/PlatformInstance.cpp
    src/Sierra/Core/PlatformInstance.h
    src/Sierra/Core/ScopeProfiler.cpp
    src/Sierra/Core/ScopeProfiler.h
    src/Sierra/Core/Version.cpp
    src/Sierra/Core/Version.h
    src/Sierra/Core/Window.cpp
    src/Sierra/Core/Window.h

    src/Sierra/Engine/RNG.cpp
    src/Sierra/Engine/RNG.h

    src/Sierra/Events/Event.h
    src/Sierra/Events/KeyEvent.h
    src/Sierra/Events/MouseEvent.h
    src/Sierra/Events/WindowEvent.h

    src/Sierra/Rendering/RenderingContext.cpp
    src/Sierra/Rendering/RenderingContext.h
    src/Sierra/Rendering/RenderingResource.h
)

# === PLATFORM-SPECIFIC SOURCE FILE LINKAGE === #
if(${DEVICE_OS} STREQUAL "Windows")
    # Add Windows-only source files
    target_sources(Sierra PRIVATE
        src/Sierra/Core/Platform/Windows/WindowsInstance.cpp
        src/Sierra/Core/Platform/Windows/WindowsInstance.h

        src/Sierra/Core/Platform/Windows/Win32Window.cpp
        src/Sierra/Core/Platform/Windows/Win32Window.h
    )

elseif(${DEVICE_OS} STREQUAL "macOS")
    # Add macOS-only source files
    target_sources(Sierra PRIVATE
        src/Sierra/Core/Platform/MacOS/MacOSInstance.mm
        src/Sierra/Core/Platform/MacOS/MacOSInstance.h

        src/Sierra/Core/Platform/MacOS/CocoaWindow.mm
        src/Sierra/Core/Platform/MacOS/CocoaWindow.h
    )

    # Enable the ObjectiveC++ language and link the Cocoa library
    enable_language(OBJCXX)
    target_link_libraries(Sierra PRIVATE "-framework Cocoa")

elseif(${DEVICE_OS} STREQUAL "Linux")
    # Add Linux-only source files
    target_sources(Sierra PRIVATE
        src/Sierra/Core/Platform/Linux/LinuxInstance.cpp
        src/Sierra/Core/Platform/Linux/LinuxInstance.h

        src/Sierra/Core/Platform/Linux/X11Window.cpp
        src/Sierra/Core/Platform/Linux/X11Window.h
    )

    # Link the X11 library
    find_package(X11 REQUIRED)
    target_link_libraries(Sierra PRIVATE ${X11_LIBRARIES})

endif()

# === GRAPHICS API LINKAGE === #
set(NO_GRAPHICS_API_SPECIFIED TRUE)
set(NO_GRAPHICS_API_SUPPORTED TRUE)

if(${SIERRA_BUILD_VULKAN})
    set(NO_GRAPHICS_API_SPECIFIED FALSE)
    if(${DEVICE_OS} STREQUAL "Windows" OR ${DEVICE_OS} STREQUAL "macOS" OR ${DEVICE_OS} STREQUAL "Linux")
        set(NO_GRAPHICS_API_SUPPORTED FALSE)
        message(STATUS "[Sierra]: Building Vulkan...")

        target_compile_definitions(Sierra PRIVATE "SR_VULKAN_SUPPORTED")
        target_compile_definitions(${PROJECT_NAME} PRIVATE "SR_VULKAN_SUPPORTED")

        # Add Vulkan-related source files
        target_sources(Sierra PRIVATE
            src/Sierra/Rendering/Platform/Vulkan/VulkanContext.cpp
            src/Sierra/Rendering/Platform/Vulkan/VulkanContext.h
            src/Sierra/Rendering/Platform/Vulkan/VulkanDevice.cpp
            src/Sierra/Rendering/Platform/Vulkan/VulkanDevice.h
            src/Sierra/Rendering/Platform/Vulkan/VulkanInstance.cpp
            src/Sierra/Rendering/Platform/Vulkan/VulkanInstance.h
            src/Sierra/Rendering/Platform/Vulkan/VulkanResource.cpp
            src/Sierra/Rendering/Platform/Vulkan/VulkanResource.h
        )

        # ========== VOLK ========== #
        set(VOLK_PULL_IN_VULKAN ON)
        add_subdirectory(vendor/Volk)
        target_link_libraries(Sierra PRIVATE volk)
        list(APPEND INCLUDE_DIRECTORIES vendor/Volk/)

        # ========== VMA ========== #
        set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
        add_subdirectory(vendor/VMA)
        target_link_libraries(Sierra PRIVATE VulkanMemoryAllocator)
        list(APPEND INCLUDE_DIRECTORIES vendor/VMA/include/)

    endif()
endif()

# === GRAPHICS API VALIDATION === #
if(${NO_GRAPHICS_API_SPECIFIED})
    message(FATAL_ERROR "No graphics API to build specified! You can enable all of them and, according to your system, the supported ones will be built automatically, or you can select just the ones you want.")

elseif(${NO_GRAPHICS_API_SUPPORTED})
    message(FATAL_ERROR "Graphics API(s) to build specified, but is/are not supported!")

endif()

# ========== GLM ========== #
add_subdirectory(vendor/glm)
target_link_libraries(Sierra PRIVATE glm)
list(APPEND INCLUDE_DIRECTORIES vendor/glm/)

# ========== SPDLOG ========== #
set(SPDLOG_ENABLE_PCH ON)
add_subdirectory(vendor/spdlog)
if(${SIERRA_BUILD_SHARED_LIBRARY} AND ${DEVICE_OS} STREQUAL "Linux")
    set_target_properties(spdlog PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")
endif()
target_link_libraries(Sierra PUBLIC spdlog::spdlog)
list(APPEND INCLUDE_DIRECTORIES vendor/spdlog/include/)

# ========== BETTER ENUMS ========== #
set(BETTER_ENUMS_DIRECTORY vendor/BetterEnums)
target_sources(Sierra PRIVATE ${BETTER_ENUMS_DIRECTORY}/enum.h)
list(APPEND INCLUDE_DIRECTORIES ${BETTER_ENUMS_DIRECTORY}/)

# === INCLUDE DIRECTORIES LINKAGE FOR ENGINE & APPLICATION === #
target_include_directories(Sierra SYSTEM PRIVATE ${INCLUDE_DIRECTORIES})
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${INCLUDE_DIRECTORIES})

# === PRECOMPILED HEADERS LINKAGE === #
target_precompile_headers(Sierra PRIVATE src/srpch.h)

# === RUN UPDATE SCRIPT === #
find_package(Python)
if(Python_FOUND)
    add_custom_target(RunUpdatePythonScript
        COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/UpdateProject.py
        COMMENT "Running project update scripts..."
    )
    add_dependencies(Sierra RunUpdatePythonScript)

else()
    message(WARNING "Python install was not found on the machine. Project update scripts cannot be run!")

endif()