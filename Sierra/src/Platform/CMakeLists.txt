target_sources(Sierra PRIVATE
    PlatformContext.h
)

if(SIERRA_PLATFORM_WINDOWS)
    add_subdirectory(Windows)
elseif(SIERRA_PLATFORM_macOS)
    add_subdirectory(macOS)
elseif(SIERRA_PLATFORM_LINUX)
    add_subdirectory(Linux)
elseif(SIERRA_PLATFORM_ANDROID)
    add_subdirectory(Android)
elseif(SIERRA_PLATFORM_iOS)
    add_subdirectory(iOS)
endif()