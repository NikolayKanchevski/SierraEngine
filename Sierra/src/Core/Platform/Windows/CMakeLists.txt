target_sources(Sierra PRIVATE
    Win32Context.cpp
    Win32Context.h
    Win32CursorManager.cpp
    Win32CursorManager.h
    Win32FileManager.cpp
    Win32FileManager.h
    Win32InputManager.cpp
    Win32InputManager.h
    Win32Window.cpp
    Win32Window.h
    Win32Screen.cpp
    Win32Screen.h
    WindowsContext.cpp
    WindowsContext.h
    WindowsWindowManager.cpp
    WindowsWindowManager.h
)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.cpp)