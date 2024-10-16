target_sources(Sierra PRIVATE
    FileErrors.cpp
    FileErrors.h
    FileManager.cpp
    FileManager.h
    FileStream.cpp
    FileStream.h
    PathErrors.cpp
    PathErrors.h
)

if(SIERRA_PLATFORM_WINDOWS)
    add_subdirectory(Windows)
elseif(SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_iOS)
    add_subdirectory(Apple)
elseif(SIERRA_PLATFORM_LINUX OR SIERRA_PLATFORM_ANDROID)
    add_subdirectory(Unix)
endif()