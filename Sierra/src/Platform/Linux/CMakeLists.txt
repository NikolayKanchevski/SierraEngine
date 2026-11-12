target_sources(Sierra PRIVATE
    LinuxContext.cpp
    LinuxContext.h
)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.cpp)