target_sources(Sierra PRIVATE
    Application.cpp
    Application.h
    CursorManager.cpp
    CursorManager.h
        EventDispatcher.hpp
    InputManager.cpp
    InputManager.h
    Key.h
    Logger.cpp
    Logger.h
    MouseButton.h
    PlatformContext.cpp
    PlatformContext.h
    ScopeProfiler.cpp
    ScopeProfiler.h
    Screen.cpp
    Screen.h
    Touch.cpp
    Touch.h
    TouchManager.cpp
    TouchManager.h
    Version.cpp
    Version.h
    Window.cpp
    Window.h
    WindowManager.cpp
    WindowManager.h
)

# Link glm
set(GLM_DIRECTORY ../../vendor/glm)
add_subdirectory(${GLM_DIRECTORY} ${GLM_DIRECTORY})
target_link_libraries(Sierra PUBLIC glm)
target_include_directories(Sierra PUBLIC ${GLM_DIRECTORY})

# Link spdlog
if(SIERRA_ENABLE_LOGGING)
    set(SPDLOG_DIRECTORY ../../vendor/spdlog)
    set(SPDLOG_ENABLE_PCH ON)
    set(SPDLOG_USE_STD_FORMAT OFF)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    add_subdirectory(${SPDLOG_DIRECTORY} ${SPDLOG_DIRECTORY})
    if(SIERRA_BUILD_SHARED_LIBRARY AND SIERRA_PLATFORM_LINUX)
        set_target_properties(spdlog PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")
    endif()
    target_link_libraries(Sierra PUBLIC spdlog)
    target_include_directories(Sierra PRIVATE ${SPDLOG_DIRECTORY}/include)
endif()

# Link platform-specific source files and libraries
if(SIERRA_PLATFORM_WINDOWS)
    add_subdirectory(Platform/Windows)
endif()
if(SIERRA_PLATFORM_macOS)
    add_subdirectory(Platform/macOS)
endif()
if(SIERRA_PLATFORM_LINUX)
    add_subdirectory(Platform/Linux)
endif()
if(SIERRA_PLATFORM_ANDROID)
    add_subdirectory(Platform/Android)
endif()
if(SIERRA_PLATFORM_iOS)
    add_subdirectory(Platform/iOS)
endif()