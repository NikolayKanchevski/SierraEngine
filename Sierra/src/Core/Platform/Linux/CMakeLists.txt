# Link source files
target_sources(Sierra PRIVATE
    LinuxInstance.cpp
    LinuxInstance.h
    X11Context.cpp
    X11Context.h
    X11CursorManager.cpp
    X11CursorManager.h
    X11Extensions.cpp
    X11Extensions.h
    X11InputManager.cpp
    X11InputManager.h
    X11Screen.cpp
    X11Screen.h
    X11Window.cpp
    X11Window.h
)

# Find X11
find_package(X11 REQUIRED)
if(NOT ${X11_FOUND})
    message(FATAL_ERROR "[Sierra]: Could not find base X11 library!")
endif()
if(NOT ${X11_xcb_xkb_FOUND})
    message(FATAL_ERROR "[Sierra]: Could not find Xkb extension of the X11 library!")
endif()
if(NOT ${X11_Xcursor_FOUND})
    message(FATAL_ERROR "[Sierra]: Could not find Xcursor extension of the X11 library!")
endif()
if(NOT ${X11_Xrandr_FOUND})
    message(FATAL_ERROR "[Sierra]: Could not find Xrandr extension of the X11 library!")
endif()

# Link libraries
target_link_libraries(Sierra PRIVATE
    ${X11_LIBRARIES}
    ${X11_xcb_xkb_LIB}
    ${X11_Xcursor_LIB}
    ${X11_Xrandr_LIB}
)