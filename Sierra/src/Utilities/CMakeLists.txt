# Link source files
target_sources(Sierra PRIVATE
    File.cpp
    File.h
    RNG.cpp
    RNG.h
    Time.cpp
    Time.h
)

if(SIERRA_PLATFORM_APPLE)
  add_subdirectory(Platform/Apple)
endif()