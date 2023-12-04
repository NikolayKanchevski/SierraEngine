# Link source files
target_sources(Sierra PRIVATE
    Device.cpp
    Device.h
    RenderingContext.cpp
    RenderingContext.h
    RenderingResource.h
)

# Link platform-specific source files and libraries
if(SIERRA_BUILD_VULKAN)
    add_subdirectory(Platform/Vulkan)
endif()
if(SIERRA_BUILD_METAL)
    add_subdirectory(Platform/Metal)
endif()