target_sources(Sierra PRIVATE
    Win32Context.cpp
    Win32Context.h
    WindowsContext.cpp
    WindowsContext.h
)

# Link system libraries (functions, which are not present on all supported versions of Windows, are loaded at runtime instead)
target_link_libraries(Sierra PRIVATE
    user32
    gdi32
    shell32
    ole32
    advapi32
    uuid
    comdlg32
    comctl32
)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.cpp)
