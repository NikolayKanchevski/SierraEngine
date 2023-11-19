# === CMAKE VERSION === #
cmake_minimum_required(VERSION 3.14)

# === INCLUDED FILES === #
include(${CMAKE_CURRENT_LIST_DIR}/cmake/PlatformDetection.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/CompilerDetection.cmake)

# === GLOBAL VARIABLES AND SETTINGS === #
set(SIERRA_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
if(SIERRA_PLATFORM_APPLE)
    enable_language(OBJCXX)
endif()

# === VERSIONING === #
set(SIERRA_VERSION_MAJOR 1)
set(SIERRA_VERSION_MINOR 0)
set(SIERRA_VERSION_PATCH 0)

# === OPTIONS === #
set(SIERRA_APPLICATION_NAME ${PROJECT_NAME} CACHE STRING "Name of the application (used for the binary and code signing).")
set(SIERRA_APPLICATION_ICON_ICO "${SIERRA_DIRECTORY}/../Media/SierraExecutableIcon.ico" CACHE FILEPATH "Absolute path to an .ico image to use as an icon for the generated binary on non-Apple platforms.")
set(SIERRA_APPLICATION_ICON_ICNS "${SIERRA_DIRECTORY}/../Media/SierraExecutableIcon.icns" CACHE FILEPATH "Absolute path to an .icns image to use as an icon for the generated binary on Apple platforms.")

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(SIERRA_DEBUG_BUILD "Wether to compile a debug build." ON)
else()
    option(SIERRA_DEBUG_BUILD "Wether to compile a debug build." OFF)
endif()

option(SIERRA_BUILD_STATIC_LIBRARY "Wether to build the engine as a static library, which is embedded in the application." OFF)
option(SIERRA_BUILD_SHARED_LIBRARY "Wether to build the engine as a shared library, which is shipped as a separate file (.dll, .dylib, etc.)" OFF)

if(SIERRA_DEBUG_BUILD)
    option(SIERRA_ENABLE_LOGGING "Wether to enable both Application and Engine logging." ON)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Wether to enable speed optimizations."  OFF)
else()
    option(SIERRA_ENABLE_LOGGING "Wether to enable both Application and Engine logging." OFF)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Wether to enable speed optimizations." ON)
endif()

option(SIERRA_BUILD_VULKAN "Wether to build Vulkan and its resources." ON)
# option(SIERRA_BUILD_OPENGL "Wether to build OpenGL and its resources." OFF) #  TODO: Build OpenGL

if(SIERRA_PLATFORM_APPLE)
    option(SIERRA_BUILD_XCODE_PROJECT "Wether to, instead of building an executable, create an Xcode project, to then use to build for macOS, iOS, iPadOS, watchOS, tvOS, or visionOS" OFF)
endif()

function(SierraBuildApplication SOURCE_FILES)
    # === CHECK IF REQUIREMENTS ARE MET === #
    if(CMAKE_CXX_STANDARD LESS 20)
        message(FATAL_ERROR "[Sierra]: Sierra requires C++ 20 or newer!")
    endif()
    if(SIERRA_COMPILER_UNKNOWN)
        message(FATAL_ERROR "[Sierra]: Sierra cannot be built with unrecognized compiler [${CMAKE_CXX_COMPILER_ID}]!")
    endif()
    if(SIERRA_PLATFORM_UNKNOWN)
        message(FATAL_ERROR "[Sierra]: Sierra cannot be built for current platform [${CMAKE_SYSTEM_NAME}]!")
    endif()

    if(SIERRA_BUILD_XCODE_PROJECT)
        set(XCODE_CMAKE_TOOL_CHAIN_FILE_PATH "${SIERRA_DIRECTORY}/vendor/ios-cmake/ios.toolchain.cmake")
        if(NOT "${CMAKE_TOOLCHAIN_FILE}" STREQUAL XCODE_CMAKE_TOOL_CHAIN_FILE_PATH)
            message(FATAL_ERROR "[Sierra]: In order to build an Xcode project, you must compile with -DCMAKE_TOOLCHAIN_FILE=${XCODE_CMAKE_TOOL_CHAIN_FILE_PATH}!")
        endif()
        if(NOT PLATFORM)
            message(FATAL_ERROR "[Sierra]: In order to build an Xcode project, you must compile with -DPLATFORM={TARGET_PLATFORM}!")
        endif()
        if(XCODE_VERSION LESS 14.0)
            message(FATAL_ERROR "[Sierra]: In order to build an Xcode project, your Xcode version must be 14.0 or newer!")
        endif()
        message(STATUS "[Sierra]: Building application into an Xcode project...")
    endif()

    # === COMPILER SETTINGS === #
    if(SIERRA_ENABLE_OPTIMIZATIONS)
        if(SIERRA_COMPILER_MSVC)
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
        elseif(SIERRA_COMPILER_CLANG OR SIERRA_COMPILER_GCC)
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
        endif()
    endif()

    if(SIERRA_COMPILER_MSVC)
        add_compile_options(/wd4250)
        add_compile_options(/wd4251)
    endif()

    # === LIBRARY GENERATION === #
    message(STATUS "[Sierra]: Building for ${SIERRA_PLATFORM}")
    if(SIERRA_BUILD_STATIC_LIBRARY)
        message(STATUS "[Sierra]: Building Sierra as static library...")
        add_library(Sierra STATIC)
    elseif(SIERRA_BUILD_SHARED_LIBRARY)
        message(STATUS "[Sierra]: Building Sierra as dynamic library...")
        add_library(Sierra SHARED)

        # Copy shared library to binary folder/bundle
        if(NOT SIERRA_PLATFORM_APPLE)
            set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
        else()
            set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${SIERRA_APPLICATION_NAME}.app/Contents/Frameworks/")
        endif()
    else()
        message(FATAL_ERROR "[Sierra]: Incorrect configuration for Sierra - either SIERRA_BUILD_STATIC_LIBRARY, or SIERRA_BUILD_SHARED_LIBRARY must be turned on!")
    endif()

    # === BINARY GENERATION === #
    if(SIERRA_PLATFORM_WINDOWS)
        # Have a folder for all temporary resources
        set(RESOURCES_FOLDER_PATH "${CMAKE_CURRENT_BINARY_DIR}/Resources")

        # Copy icon to resources folder
        set(ICON_OUTPUT_PATH "${RESOURCES_FOLDER_PATH}/${SIERRA_APPLICATION_NAME}Icon.ico")
        configure_file(${SIERRA_APPLICATION_ICON_ICO} ${ICON_OUTPUT_PATH} COPYONLY)

        # Define RC file data
        set(RC_FILE_DATA "
            IDR_MAINFRAME ICON
            \"${SIERRA_APPLICATION_NAME}Icon.ico\"
        ")

        # Create and write to RC file
        set(RC_FILE_PATH "${RESOURCES_FOLDER_PATH}/Application.rc")
        file(WRITE ${RC_FILE_PATH} "\n")

        # Create executable
        add_executable(${SIERRA_APPLICATION_NAME} ${SOURCE_FILES} ${RC_FILE_PATH})
    elseif(SIERRA_PLATFORM_LINUX)
        # Have a folder for all temporary resources
        set(RESOURCES_FOLDER_PATH "${CMAKE_CURRENT_BINARY_DIR}/Resources")

        # Copy icon to resources folder
        set(ICON_OUTPUT_PATH "${RESOURCES_FOLDER_PATH}/${SIERRA_APPLICATION_NAME}Icon.ico")
        configure_file(${SIERRA_APPLICATION_ICON_ICO} ${ICON_OUTPUT_PATH} COPYONLY)

        # Define desktop file data
        set(DESKTOP_FILE_DATA "
            [Desktop Entry]
            Encoding=UTF-8
            Version=${SIERRA_VERSION_MAJOR}.${SIERRA_VERSION_MINOR}.${SIERRA_VERSION_PATCH}
            Type=Application
            Terminal=false
            Exec=${CMAKE_CURRENT_BINARY_DIR}/${SIERRA_APPLICATION_NAME}
            Name=${SIERRA_APPLICATION_NAME}
            Icon=${ICON_OUTPUT_PATH}
        ")

        # Create and write to desktop file
        set(DESKTOP_FILE_PATH "$ENV{HOME}/.local/share/applications/${SIERRA_APPLICATION_NAME}.desktop")
        file(WRITE ${DESKTOP_FILE_PATH} ${DESKTOP_FILE_DATA})

        # Create executable
        add_executable(${SIERRA_APPLICATION_NAME} ${SOURCE_FILES})
    elseif(SIERRA_PLATFORM_APPLE)
        # Create executable and set its properties
        add_executable(${SIERRA_APPLICATION_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
        set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES
            BUNDLE TRUE
            MACOSX_BUNDLE_BUNDLE_NAME ${SIERRA_APPLICATION_NAME}
            MACOSX_BUNDLE_GUI_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}"
            MACOSX_BUNDLE_PRODUCT_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}"
            MACOSX_BUNDLE_BUNDLE_VERSION "${SIERRA_VERSION_MAJOR}.${SIERRA_VERSION_MINOR}.${SIERRA_VERSION_PATCH}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${SIERRA_VERSION_MAJOR}"
        )

        # Copy .icns file to application's resources
        set(ICON_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/${SIERRA_APPLICATION_NAME}.app/Resources/${SIERRA_APPLICATION_NAME}Icon.icns")
        configure_file(${SIERRA_APPLICATION_ICON_ICNS} ${ICON_OUTPUT_PATH} COPYONLY)

        # Link icon with the application
        set_source_files_properties(${ICON_OUTPUT_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        target_sources(${SIERRA_APPLICATION_NAME} PRIVATE ${ICON_OUTPUT_PATH})

        if(SIERRA_PLATFORM_iOS)
            # Set Xcode project's info.plist
            set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES
                MACOSX_BUNDLE_INFO_PLIST ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/plist.in
            )
        endif()
    else()
        add_executable(${SIERRA_APPLICATION_NAME} ${SOURCE_FILES})
    endif()

    # === EXPORT ENGINE SYMBOLS ===
    if(SIERRA_BUILD_STATIC_LIBRARY)
        target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_BUILD_STATIC_LIBRARY")
    elseif(SIERRA_BUILD_SHARED_LIBRARY)
        target_compile_definitions(Sierra PRIVATE "SR_BUILD_SHARED_LIBRARY")
        target_compile_definitions(Sierra PRIVATE "SR_USE_SHARED_LIBRARY")
        target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_USE_SHARED_LIBRARY")
    endif()

    # === CREATE DEFINITIONS === #
    if(SIERRA_DEBUG_BUILD)
        target_compile_definitions(Sierra PRIVATE "SR_DEBUG")
        target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_DEBUG")
    endif()

    if(SIERRA_ENABLE_LOGGING)
        target_compile_definitions(Sierra PRIVATE "SR_ENABLE_LOGGING")
        target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_ENABLE_LOGGING")
    endif()

    # === PLATFORM-SPECIFIC LIBRARY LINKAGE === #
    if(SIERRA_PLATFORM_LINUX)
        find_package(X11 REQUIRED)
        if(NOT ${X11_FOUND})
            message(FATAL_ERROR "[Sierra]: Could not find base X11 library!")
        endif()
        if(NOT ${X11_xcb_xkb_FOUND})
            message(FATAL_ERROR "[Sierra]: Could not find Xkb extension of the X11 library!")
        endif()
        if(NOT ${X11_Xcursor_FOUND})
            message(FATAL_ERROR "[Sierra]: Could not find Xcursor extension of the X11 library!")
        endif()
        if(NOT ${X11_Xrandr_FOUND})
            message(FATAL_ERROR "[Sierra]: Could not find Xrandr extension of the X11 library!")
        endif()
        target_link_libraries(Sierra PRIVATE ${X11_LIBRARIES} ${X11_xcb_xkb_LIB} ${X11_Xcursor_LIB} ${X11_Xrandr_LIB})
    elseif(SIERRA_PLATFORM_MACOS)
        target_link_libraries(Sierra PRIVATE "-framework Cocoa")
    elseif(SIERRA_PLATFORM_iOS)
        target_link_libraries(Sierra PRIVATE "-framework UIKit")
        target_link_libraries(Sierra PRIVATE "-framework QuartzCore")
    endif()

    # === SOURCE FILE LINKAGE === #
    set(INCLUDE_DIRECTORIES ${SIERRA_DIRECTORY}/src/ ${SIERRA_DIRECTORY}/include/)
    target_sources(Sierra PRIVATE
        ${SIERRA_DIRECTORY}/include/Sierra.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/API.h

        ${SIERRA_DIRECTORY}/src/Sierra/Core/CursorManager.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/CursorManager.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/InputManager.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/InputManager.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Key.h
        ${SIERRA_DIRECTORY}/src/Sierra/Application.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Application.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Logger.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Logger.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/MouseButton.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/PlatformInstance.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/PlatformInstance.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/ScopeProfiler.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/ScopeProfiler.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Screen.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Screen.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Touch.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Touch.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/TouchManager.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/TouchManager.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Version.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Version.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Window.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/Window.h
        ${SIERRA_DIRECTORY}/src/Sierra/Core/WindowManager.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Core/WindowManager.h

        ${SIERRA_DIRECTORY}/src/Sierra/Engine/RNG.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Engine/RNG.h
        ${SIERRA_DIRECTORY}/src/Sierra/Engine/Time.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Engine/Time.h

        ${SIERRA_DIRECTORY}/src/Sierra/Events/CursorEvent.h
        ${SIERRA_DIRECTORY}/src/Sierra/Events/Event.h
        ${SIERRA_DIRECTORY}/src/Sierra/Events/InputEvent.h
        ${SIERRA_DIRECTORY}/src/Sierra/Events/TouchEvent.h
        ${SIERRA_DIRECTORY}/src/Sierra/Events/WindowEvent.h

        ${SIERRA_DIRECTORY}/src/Sierra/Rendering/RenderingContext.cpp
        ${SIERRA_DIRECTORY}/src/Sierra/Rendering/RenderingContext.h
        ${SIERRA_DIRECTORY}/src/Sierra/Rendering/RenderingResource.h
    )

    # === PLATFORM-SPECIFIC SOURCE FILE LINKAGE === #
    if(SIERRA_PLATFORM_WINDOWS)
        # Add Windows-only source files
        target_sources(Sierra PRIVATE
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/Win32CursorManager.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/Win32CursorManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/WindowsInstance.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/WindowsInstance.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/Win32InputManager.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/Win32InputManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/Win32Window.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Windows/Win32Window.h
        )
    elseif(SIERRA_PLATFORM_LINUX)
        # Add Linux-only source files
        target_sources(Sierra PRIVATE
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/LinuxInstance.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/LinuxInstance.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Context.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Context.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11CursorManager.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11CursorManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Extensions.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Extensions.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11InputManager.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11InputManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Screen.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Screen.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Window.cpp
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/Linux/X11Window.h
        )
    elseif(SIERRA_PLATFORM_MACOS)
        # Add macOS-only source files
        target_sources(Sierra PRIVATE
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaContext.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaContext.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaCursorManager.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaCursorManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaInputManager.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaInputManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaScreen.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaScreen.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaWindow.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/CocoaWindow.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/MacOSInstance.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/MacOS/MacOSInstance.h
        )
    elseif(SIERRA_PLATFORM_iOS)
        # Add iOS-only source files
        target_sources(Sierra PRIVATE
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/iOSInstance.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/iOSInstance.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitContext.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitContext.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitScreen.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitScreen.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitSelectorBridge.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitSelectorBridge.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitTemporaryCreateInfoStorage.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitTemporaryCreateInfoStorage.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitTouchManager.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitTouchManager.h
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitWindow.mm
            ${SIERRA_DIRECTORY}/src/Sierra/Core/Platform/iOS/UIKitWindow.h
        )
    endif()

    # === GRAPHICS API LINKAGE === #
    set(NO_GRAPHICS_API_SPECIFIED TRUE)
    set(NO_GRAPHICS_API_SUPPORTED TRUE)

    if(SIERRA_BUILD_VULKAN)
        set(NO_GRAPHICS_API_SPECIFIED FALSE)
        if(
            SIERRA_PLATFORM_WINDOWS OR
            SIERRA_PLATFORM_MACOS OR
            SIERRA_PLATFORM_LINUX OR
            SIERRA_PLATFORM_iOS
        )
            if(NOT SIERRA_BUILD_XCODE_PROJECT)
                find_package(Vulkan QUIET)
                if(SIERRA_PLATFORM_MACOS)
                    find_library(MoltenVK_LIBRARIES NAMES MoltenVK)
                    if(MoltenVKLibrary)
                        list(APPEND Vulkan_LIBRARIES ${MoltenVK_LIBRARIES})
                    endif()
                endif()
            else()
                # Add MoltenVK's prerequisites
                set(Vulkan_LIBRARIES
                    "-framework CoreGraphics"
                    "-framework Metal"
                    "-framework Foundation"
                    "-framework QuartzCore"
                    "-framework IOSurface"
                )
                if(SIERRA_PLATFORM_MACOS)
                    list(APPEND Vulkan_LIBRARIES "-framework IOKit")
                elseif(SIERRA_PLATFORM_iOS)
                    list(APPEND Vulkan_LIBRARIES "-framework UIKit")
                endif()

                # Manually link MoltenVK, because find_package and find_library do not work in Xcode environment
                if(NOT DEFINED VULKAN_SDK_PATH)
                    message(FATAL_ERROR "[Sierra]: When building an Xcode project, VULKAN_SDK_PATH must be set manually!")
                else()
                    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/MoltenVK/include/")
                    if(${PLATFORM} STREQUAL "OS64")
                        list(APPEND Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/MoltenVK/MoltenVK.xcframework/ios-arm64/libMoltenVK.a")
                    elseif(${PLATFORM} STREQUAL "SIMULATORARM64" OR ${PLATFORM} STREQUAL "SIMULATOR64COMBINED")
                        list(APPEND Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/MoltenVK/MoltenVK.xcframework/ios-arm64_x86_64-simulator/libMoltenVK.a")
                    elseif(${PLATFORM} STREQUAL "MAC" OR ${PLATFORM} STREQUAL "MAC_ARM64" OR ${PLATFORM} STREQUAL "MAC_UNIVERSAL" OR ${PLATFORM} STREQUAL "MAC_CATALYST" OR ${PLATFORM} STREQUAL "MAC_CATALYST_ARM64")
                        list(APPEND Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/MoltenVK/MoltenVK.xcframework/macos-arm64_x86_64/libMoltenVK.a")
                    endif()
                endif()
            endif()

            if(Vulkan_LIBRARIES AND Vulkan_INCLUDE_DIRS)
                set(NO_GRAPHICS_API_SUPPORTED FALSE)
                message(STATUS "[Sierra]: Building Vulkan...")

                # Link Vulkan
                target_link_libraries(Sierra PRIVATE ${Vulkan_LIBRARIES})
                list(APPEND INCLUDE_DIRECTORIES ${Vulkan_INCLUDE_DIRS})

                # Set Vulkan definitions
                target_compile_definitions(Sierra PRIVATE "SR_VULKAN_SUPPORTED")
                target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_VULKAN_SUPPORTED")

                # Add Vulkan source files
                target_sources(Sierra PRIVATE
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanContext.cpp
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanContext.h
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanDevice.cpp
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanDevice.h
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanInstance.cpp
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanInstance.h
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanResource.cpp
                    ${SIERRA_DIRECTORY}/src/Sierra/Rendering/Platform/Vulkan/VulkanResource.h
                )

                # Link VMA
                set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
                add_subdirectory(${SIERRA_DIRECTORY}/vendor/VMA ${SIERRA_DIRECTORY}/vendor/VMA)
                target_link_libraries(Sierra PRIVATE VulkanMemoryAllocator)
                list(APPEND INCLUDE_DIRECTORIES ${SIERRA_DIRECTORY}/vendor/VMA/include/)
            else()
                message(WARNING "[Sierra]: Vulkan is supported on the system and was requested to be built, but it could not be found! Did you install the Vulkan SDK from https://vulkan.lunarg.com/sdk/home#mac?")
            endif()
        endif()
    endif()

    # === GRAPHICS API VALIDATION === #
    if(NO_GRAPHICS_API_SPECIFIED)
        message(FATAL_ERROR "No graphics API to build specified! You can enable all of them and, according to your system, the supported ones will be built automatically, or you can select just the ones you want.")
    elseif(NO_GRAPHICS_API_SUPPORTED)
        message(FATAL_ERROR "Graphics API(s) to build specified, but none were built!")
    endif()

    # === EXTERNAL LIBRARY LINKAGE ===
    # Link GLM
    add_subdirectory(${SIERRA_DIRECTORY}/vendor/glm ${SIERRA_DIRECTORY}/vendor/glm)
    target_link_libraries(Sierra PRIVATE glm)
    list(APPEND INCLUDE_DIRECTORIES ${SIERRA_DIRECTORY}/vendor/glm/)

    # Linkg spdlog
    if (SIERRA_ENABLE_LOGGING)
        set(SPDLOG_ENABLE_PCH ON)
        set(SPDLOG_USE_STD_FORMAT OFF)
        set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
        add_subdirectory(${SIERRA_DIRECTORY}/vendor/spdlog ${SIERRA_DIRECTORY}/vendor/spdlog)
        if(SIERRA_BUILD_SHARED_LIBRARY AND SIERRA_PLATFORM_LINUX)
            set_target_properties(spdlog PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")
        endif()
        target_link_libraries(Sierra PUBLIC spdlog)
        list(APPEND INCLUDE_DIRECTORIES ${SIERRA_DIRECTORY}/vendor/spdlog/include/)
    endif()

    # Link BetterEnums
    set(BETTER_ENUMS_DIRECTORY ${SIERRA_DIRECTORY}/vendor/BetterEnums)
    target_sources(Sierra PRIVATE ${BETTER_ENUMS_DIRECTORY}/enum.h)
    list(APPEND INCLUDE_DIRECTORIES ${BETTER_ENUMS_DIRECTORY}/)

    # === LINK ENGINE LIBRARY WITH APPLICATION === #
    target_include_directories(Sierra SYSTEM PRIVATE ${INCLUDE_DIRECTORIES})
    target_include_directories(${SIERRA_APPLICATION_NAME} SYSTEM PRIVATE ${INCLUDE_DIRECTORIES})
    target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE Sierra)

    # === LINK PRECOMPILED HEADERS === #
    target_precompile_headers(Sierra PRIVATE ${SIERRA_DIRECTORY}/src/srpch.h)

    # === RUN UPDATE SCRIPT === #
    find_package(Python)
    if(Python_FOUND)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} PRE_BUILD
            COMMAND ${Python_EXECUTABLE} ${SIERRA_DIRECTORY}/scripts/UpdateProject.py
        )
        message(STATUS "[Sierra]: Running project update scripts...")
    else()
        message(WARNING "[Sierra]: Python install was not found on the machine. Project update scripts cannot be run!")
    endif()

    # === PROJECT GENERATION CONFIGURATION (LAST BECAUSE WE MAY NEED TO MODIFY ALL EXISTING TARGETS) === #
    if(SIERRA_BUILD_XCODE_PROJECT)
        function(get_all_targets var)
            set(targets)
            get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
            set(${var} ${targets} PARENT_SCOPE)
        endfunction()

        macro(get_all_targets_recursive targets dir)
            get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
            foreach(subdir ${subdirectories})
                get_all_targets_recursive(${targets} ${subdir})
            endforeach()

            get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
            list(APPEND ${targets} ${current_targets})
        endmacro()

        get_all_targets(ALL_TARGETS)
        macro(set_global_xcode_property XCODE_PROPERTY XCODE_VALUE XCODE_VARIANT)
            foreach(TARGET ${ALL_TARGETS})
                set_xcode_property(${TARGET} ${XCODE_PROPERTY} ${XCODE_VALUE} ${XCODE_VARIANT})
            endforeach()
        endmacro()

        set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "YES" "Debug")
        set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "NO" "MinSizeRel")
        set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "YES" "RelWithDebInfo")
        set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "NO" "Release")

        set_global_xcode_property(COPY_PHASE_STRIP "NO" "Debug")
        set_global_xcode_property(COPY_PHASE_STRIP "YES" "MinSizeRel")
        set_global_xcode_property(COPY_PHASE_STRIP "NO" "RelWithDebInfo")
        set_global_xcode_property(COPY_PHASE_STRIP "YES" "Release")

        if(SIERRA_ENABLE_OPTIMIZATIONS)
            set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "0" "Debug")
            set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "s" "MinSizeRel")
            set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "3" "RelWithDebInfo")
            set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "3" "Release")
        endif()

        set_global_xcode_property(IPHONEOS_DEPLOYMENT_TARGET "13.0" "All")
        set_global_xcode_property(MACOSX_DEPLOYMENT_TARGET "10.13.6" "All")

        set_xcode_property(Sierra CLANG_ENABLE_OBJC_ARC "NO" "All")
        set_xcode_property(${SIERRA_APPLICATION_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}" "All")
    endif()
endfunction()