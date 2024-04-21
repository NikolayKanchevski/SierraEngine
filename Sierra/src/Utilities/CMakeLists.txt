target_sources(Sierra PRIVATE
    RNG.cpp
    RNG.h
    Time.cpp
    Time.h
)

if(SIERRA_PLATFORM_APPLE)
  add_subdirectory(Platform/Apple)
endif()