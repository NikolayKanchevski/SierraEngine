target_sources(Sierra PRIVATE
    Win32Context.cpp
    Win32Context.h
    WindowsContext.cpp
    WindowsContext.h
)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.cpp)