# Add macOS source files
enable_language(OBJCXX)
target_sources(Sierra PRIVATE
        VulkanMacOSSurface.mm
        VulkanMacOSSurface.h
)