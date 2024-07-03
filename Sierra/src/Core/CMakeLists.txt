target_sources(Sierra PRIVATE
    Application.cpp
    Application.h
    CursorManager.cpp
    CursorManager.h
    EventDispatcher.hpp
    FileManager.cpp
    FileManager.h
    InputManager.cpp
    InputManager.h
    Logger.cpp
    Logger.h
    PlatformContext.cpp
    PlatformContext.h
    RNG.cpp
    RNG.h
    ScopeProfiler.cpp
    ScopeProfiler.h
    Screen.cpp
    Screen.h
    Time.cpp
    Time.h
    Touch.cpp
    Touch.h
    TouchManager.cpp
    TouchManager.h
    Version.hpp
    Window.cpp
    Window.h
    WindowManager.cpp
    WindowManager.h
)

# Link glm
set(GLM_DIRECTORY_PATH ../../vendor/glm)
add_subdirectory(${GLM_DIRECTORY_PATH} ${GLM_DIRECTORY_PATH})
target_link_libraries(Sierra PUBLIC glm)
target_include_directories(Sierra PUBLIC ${GLM_DIRECTORY_PATH})

# Link fmt
set(FMT_PEDANTIC ON)
set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
set(FMT_TEST OFF)
set(FMT_OS OFF)
set(FMT_DIRECTORY_PATH ../../vendor/fmt)
add_subdirectory(${FMT_DIRECTORY_PATH} ${FMT_DIRECTORY_PATH})
target_link_libraries(Sierra PUBLIC fmt)
target_include_directories(Sierra PUBLIC ${FMT_DIRECTORY_PATH}/include)

# Link spdlog
if(SIERRA_ENABLE_LOGGING)
    set(SPDLOG_DIRECTORY_PATH ../../vendor/spdlog)
    set(SPDLOG_ENABLE_PCH ON)
    set(SPDLOG_FMT_EXTERNAL ON)
    set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
    add_subdirectory(${SPDLOG_DIRECTORY_PATH} ${SPDLOG_DIRECTORY_PATH})
    if(SIERRA_BUILD_SHARED_LIBRARY AND SIERRA_PLATFORM_LINUX)
        set_target_properties(spdlog PROPERTIES COMPILE_FLAGS "-fPIC" LINK_FLAGS "-fPIC")
    endif()
    target_link_libraries(Sierra PUBLIC spdlog)
    target_include_directories(Sierra PRIVATE ${SPDLOG_DIRECTORY_PATH}/include)
endif()

# Link platform-specific source files and libraries
if(SIERRA_PLATFORM_WINDOWS)
    add_subdirectory(Platform/Windows)
endif()
if(SIERRA_PLATFORM_macOS)
    add_subdirectory(Platform/macOS)
    add_subdirectory(Platform/Apple)
endif()
if(SIERRA_PLATFORM_LINUX)
    add_subdirectory(Platform/Linux)
endif()
if(SIERRA_PLATFORM_ANDROID)
    add_subdirectory(Platform/Android)
endif()
if(SIERRA_PLATFORM_iOS)
    add_subdirectory(Platform/iOS)
    add_subdirectory(Platform/Apple)
endif()