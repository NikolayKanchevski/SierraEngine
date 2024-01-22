# Try to link Vulkan
if(SIERRA_PLATFORM_WINDOWS OR SIERRA_PLATFORM_LINUX)
    find_package(Vulkan QUIET)
elseif(SIERRA_PLATFORM_macOS)
    find_package(Vulkan QUIET)
    find_library(MoltenVK_LIBRARIES NAMES MoltenVK)
    if(MoltenVKLibrary)
        list(APPEND Vulkan_LIBRARIES ${MoltenVK_LIBRARIES})
    endif()
elseif(SIERRA_PLATFORM_ANDROID)
    add_definitions(-DVK_USE_PLATFORM_ANDROID_KHR=1)
    list(APPEND Vulkan_LIBRARIES vulkan)
elseif(SIERRA_PLATFORM_iOS)
    # Add MoltenVK's prerequisites
    set(Vulkan_LIBRARIES
        "-framework CoreGraphics"
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
        "-framework IOSurface"
        "-framework UIKit"
    )

    # Manually link MoltenVK, because find_package and find_library do not work in Xcode environment
    if(NOT DEFINED VULKAN_SDK_PATH)
        message(FATAL_ERROR "[Sierra]: When building Vulkan for iOS, VULKAN_SDK_PATH must be set manually!")
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
if(Vulkan_LIBRARIES)
    message(STATUS "[Sierra]: Building Vulkan...")

    # Link Vulkan
    target_link_libraries(Sierra PRIVATE ${Vulkan_LIBRARIES})
    target_include_directories(Sierra PRIVATE ${Vulkan_INCLUDE_DIRS})

    # Set Vulkan definitions
    target_compile_definitions(Sierra PUBLIC "SR_VULKAN_SUPPORTED")

    # Add Vulkan source files
    target_sources(Sierra PRIVATE
        VulkanBuffer.cpp
        VulkanBuffer.h
        VulkanCommandBuffer.cpp
        VulkanCommandBuffer.h
        VulkanContext.cpp
        VulkanContext.h
        VulkanDescriptorSets.cpp
        VulkanDescriptorSets.h
        VulkanDevice.cpp
        VulkanDevice.h
        VulkanGraphicsPipeline.cpp
        VulkanGraphicsPipeline.h
        VulkanImage.cpp
        VulkanImage.h
        VulkanInstance.cpp
        VulkanInstance.h
        VulkanPipeline.cpp
        VulkanPipeline.h
        VulkanPipelineLayout.cpp
        VulkanPipelineLayout.h
        VulkanRenderPass.cpp
        VulkanRenderPass.h
        VulkanResource.cpp
        VulkanResource.h
        VulkanShader.cpp
        VulkanShader.h
        VulkanSwapchain.cpp
        VulkanSwapchain.h
    )

    # Add per-platform source files
    if(SIERRA_PLATFORM_WINDOWS)
        add_subdirectory(Platform/Windows)
    elseif(SIERRA_PLATFORM_LINUX)
        add_subdirectory(Platform/Linux)
    elseif(SIERRA_PLATFORM_macOS)
        add_subdirectory(Platform/macOS)
    elseif(SIERRA_PLATFORM_ANDROID)
        add_subdirectory(Platform/Android)
    elseif(SIERRA_PLATFORM_iOS)
        add_subdirectory(Platform/iOS)
    endif()

    # Link VMA
    set(VMA_DIRECTORY ../../../../vendor/VMA)
    set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
    add_subdirectory(${VMA_DIRECTORY} ${VMA_DIRECTORY})
    target_link_libraries(Sierra PRIVATE VulkanMemoryAllocator)
    target_include_directories(Sierra SYSTEM PRIVATE ${VMA_DIRECTORY}/include/)
else()
    message(WARNING "[Sierra]: Vulkan is supported on the system and was requested to be built, but it could not be found! Did you install the Vulkan SDK from https://vulkan.lunarg.com/sdk/home#mac?")
endif()