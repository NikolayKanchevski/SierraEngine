# Try to link Vulkan
if(NOT SIERRA_BUILD_XCODE_PROJECT)
    find_package(Vulkan QUIET)
    if(SIERRA_PLATFORM_macOS)
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
    if(SIERRA_PLATFORM_macOS)
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

# Check if it succeeded and proceed
if(Vulkan_LIBRARIES AND Vulkan_INCLUDE_DIRS)
    message(STATUS "[Sierra]: Building Vulkan...")

    # Link Vulkan
    target_link_libraries(Sierra PRIVATE ${Vulkan_LIBRARIES})
    target_include_directories(Sierra PRIVATE ${Vulkan_INCLUDE_DIRS})

    # Set Vulkan definitions
    target_compile_definitions(Sierra PUBLIC "SR_VULKAN_SUPPORTED")

    # Add Vulkan source files
    target_sources(Sierra PRIVATE
        VulkanContext.cpp
        VulkanContext.h
        VulkanDevice.cpp
        VulkanDevice.h
        VulkanInstance.cpp
        VulkanInstance.h
        VulkanResource.cpp
        VulkanResource.h
    )

    # Link VMA
    set(VMA_DIRECTORY ../../../../vendor/VMA)
    set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
    add_subdirectory(${VMA_DIRECTORY} SYSTEM ${VMA_DIRECTORY})
    target_link_libraries(Sierra PRIVATE VulkanMemoryAllocator)
    target_include_directories(Sierra SYSTEM PRIVATE ${VMA_DIRECTORY}/include/)
else()
    message(WARNING "[Sierra]: Vulkan is supported on the system and was requested to be built, but it could not be found! Did you install the Vulkan SDK from https://vulkan.lunarg.com/sdk/home#mac?")
endif()