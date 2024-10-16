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
        set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/iOS/include/")
        if(${PLATFORM} STREQUAL "OS64")
            list(APPEND Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/iOS/lib/MoltenVK.xcframework/ios-arm64/MoltenVK.framework/MoltenVK")
        elseif(${PLATFORM} STREQUAL "SIMULATORARM64" OR ${PLATFORM} STREQUAL "SIMULATOR64COMBINED")
            list(APPEND Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/iOS/lib/MoltenVK.xcframework/ios-arm64_x86_64-simulator/MoltenVK.framework/MoltenVK")
        elseif(${PLATFORM} STREQUAL "MAC" OR ${PLATFORM} STREQUAL "MAC_ARM64" OR ${PLATFORM} STREQUAL "MAC_UNIVERSAL" OR ${PLATFORM} STREQUAL "MAC_CATALYST" OR ${PLATFORM} STREQUAL "MAC_CATALYST_ARM64")
            list(APPEND Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/iOS/lib/MoltenVK.xcframework/macos-arm64_x86_64/MoltenVK.framework/MoltenVK")
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
    target_compile_definitions(Sierra PUBLIC "SR_VULKAN_SUPPORTED=1")

    # Add Vulkan source files
    target_sources(Sierra PRIVATE
        VulkanBuffer.cpp
        VulkanBuffer.h
        VulkanCommandBuffer.cpp
        VulkanCommandBuffer.h
        VulkanComputePipeline.cpp
        VulkanComputePipeline.h
        VulkanContext.cpp
        VulkanContext.h
        VulkanDevice.cpp
        VulkanDevice.h
        VulkanGraphicsPipeline.cpp
        VulkanGraphicsPipeline.h
        VulkanImage.cpp
        VulkanImage.h
        VulkanQueue.cpp
        VulkanQueue.h
        VulkanRenderPass.cpp
        VulkanRenderPass.h
        VulkanResource.h
        VulkanResourceTable.h
        VulkanResourceTable.cpp
        VulkanResultHandler.h
        VulkanResultHandler.cpp
        VulkanSampler.cpp
        VulkanSampler.h
        VulkanShader.cpp
        VulkanShader.h
        VulkanSwapchain.cpp
        VulkanSwapchain.h
    )

    if(SIERRA_PLATFORM_WINDOWS)
        add_subdirectory(Windows)
    elseif(SIERRA_PLATFORM_LINUX)
        add_subdirectory(Linux)
    elseif(SIERRA_PLATFORM_APPLE)
        add_subdirectory(Apple)
    elseif(SIERRA_PLATFORM_ANDROID)
        add_subdirectory(Android)
    endif()

    # Link VMA
    set(VMA_DIRECTORY_PATH ../../../vendor/VMA/)
    set(VMA_STATIC_VULKAN_FUNCTIONS OFF)
    add_subdirectory(${VMA_DIRECTORY_PATH} ${VMA_DIRECTORY_PATH})
    target_link_libraries(Sierra PRIVATE VulkanMemoryAllocator)
    target_include_directories(Sierra SYSTEM PRIVATE ${VMA_DIRECTORY_PATH}/include/)
else()
    message(WARNING "[Sierra]: Vulkan is supported on the system and was requested to be built, but it could not be found! Did you install the Vulkan SDK from https://vulkan.lunarg.com/sdk/home#mac?")
endif()