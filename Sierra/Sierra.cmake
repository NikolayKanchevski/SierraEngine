# === CMAKE SETTINGS === #
cmake_minimum_required(VERSION 3.14)
set(CMAKE_SUPPRESS_REGENERATION true)

# === INCLUDED FILES === #
set(SIERRA_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
include(${SIERRA_DIRECTORY}/cmake/PlatformDetection.cmake)
include(${SIERRA_DIRECTORY}/cmake/CompilerDetection.cmake)

# === OPTIONS === $
set(SIERRA_APPLICATION_NAME ${PROJECT_NAME} CACHE STRING "Name of the application (used for the binary and code signing).")
set(SIERRA_APPLICATION_VERSION_MAJOR 1 CACHE STRING "Major version of the application.")
set(SIERRA_APPLICATION_VERSION_MINOR 0 CACHE STRING "Minor version of the application.")
set(SIERRA_APPLICATION_VERSION_PATCH 0 CACHE STRING "Patch number of the application.")
set(SIERRA_APPLICATION_ICON_ICO "../Media/SierraExecutableIcon.ico" CACHE FILEPATH "Absolute path to an .ico image to use as an icon for the generated binary on non-Apple platforms.")
set(SIERRA_APPLICATION_ICON_ICNS "../Media/SierraExecutableIcon.icns" CACHE FILEPATH "Absolute path to an .icns image to use as an icon for the generated binary on Apple platforms.")

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(SIERRA_DEBUG_BUILD "Wether to compile a debug build." ON)
else()
    option(SIERRA_DEBUG_BUILD "Wether to compile a debug build." OFF)
endif()

option(SIERRA_BUILD_STATIC_LIBRARY "Wether to build the engine as a static library, which is embedded in the application." OFF)
option(SIERRA_BUILD_SHARED_LIBRARY "Wether to build the engine as a shared library, which is shipped as a separate file (.dll, .dylib, etc.)" OFF)

if(SIERRA_DEBUG_BUILD)
    option(SIERRA_ENABLE_LOGGING "Wether to enable Application and Engine logging." ON)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Wether to enable speed optimizations."  OFF)
else()
    option(SIERRA_ENABLE_LOGGING "Wether to enable both Application and Engine logging." OFF)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Wether to enable speed optimizations." ON)
endif()

if(SIERRA_PLATFORM_WINDOWS OR SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_LINUX OR SIERRA_PLATFORM_ANDROID OR SIERRA_PLATFORM_iOS)
    option(SIERRA_BUILD_VULKAN "Wether to build Vulkan and its resources." ON)
endif()
if(SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_iOS)
    option(SIERRA_BUILD_METAL "Wether to build Metal and its resources." ON)
endif()

if(SIERRA_PLATFORM_APPLE)
    option(SIERRA_BUILD_XCODE_PROJECT "Wether to, instead of building an executable, create an Xcode project, to then use to build for macOS, iOS, iPadOS, watchOS, tvOS, or visionOS." OFF)
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

    # === ENABLE ADDITIONAL LANGUAGES === #
    if(SIERRA_PLATFORM_APPLE)
        enable_language(OBJCXX)
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

    # == OUTPUT GENERATION === #
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
        file(WRITE ${RC_FILE_PATH} ${RC_FILE_DATA})

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
            Version=${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}
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
            MACOSX_BUNDLE_BUNDLE_VERSION "${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}"
            MACOSX_BUNDLE_SHORT_VERSION_STRING "${SIERRA_APPLICATION_VERSION_MAJOR}"
        )

        # Link icon with the application
        set_source_files_properties(${SIERRA_APPLICATION_ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        target_sources(${SIERRA_APPLICATION_NAME} PRIVATE ${SIERRA_APPLICATION_ICON_ICNS})

        if(SIERRA_PLATFORM_iOS)
            # Set Xcode project's info.plist
            set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${SIERRA_DIRECTORY}/src/Core/Platform/iOS/config/plist.in)
        endif()
    elseif(SIERRA_PLATFORM_ANDROID)
        add_library(${SIERRA_APPLICATION_NAME} SHARED ${SOURCE_FILES})
    endif()

    # === ENGINE LINKING === #
    add_subdirectory(${SIERRA_DIRECTORY}/src ${SIERRA_DIRECTORY}/src)
    target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE Sierra)
    target_include_directories(${SIERRA_APPLICATION_NAME} PRIVATE ${SIERRA_DIRECTORY}/include/)

    # === RUN UPDATE SCRIPT === #
    find_package(Python)
    if(Python_FOUND)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} POST_BUILD COMMAND ${Python_EXECUTABLE} ${SIERRA_DIRECTORY}/scripts/UpdateProject.py)
        message(STATUS "[Sierra]: Running project update scripts...")
    else()
        message(WARNING "[Sierra]: Python install was not found on the machine. Project update scripts cannot be run!")
    endif()

    # === PROJECT GENERATION (DONE LAST, AS WE MAY NEED TO MODIFY ALL EXISTING TARGETS) === #
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