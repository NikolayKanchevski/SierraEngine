target_sources(Sierra PRIVATE
    GameActivityContext.cpp
    GameActivityContext.h
    AndroidContext.h
    AndroidContext.cpp
    GameActivityScreen.h
    GameActivityScreen.cpp
    GameActivityTouchManager.h
    GameActivityTouchManager.cpp
    GameActivityWindow.h
    GameActivityWindow.cpp
)

# Find game activity
find_package(game-activity REQUIRED CONFIG)
get_target_property(game-activity_INCLUDE_DIRECTORIES game-activity::game-activity INTERFACE_INCLUDE_DIRECTORIES)

# Enable native activity
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u \ Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build app_glue
set(APP_GLUE_DIRECTORY "${game-activity_INCLUDE_DIRECTORIES}/game-activity/native_app_glue")
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE android_main.cpp ${APP_GLUE_DIRECTORY}/android_native_app_glue.h ${APP_GLUE_DIRECTORY}/android_native_app_glue.c ${game-activity_INCLUDE_DIRECTORIES}/game-activity/GameActivity.h ${game-activity_INCLUDE_DIRECTORIES}/game-activity/GameActivity.cpp)
target_sources(Sierra PRIVATE ${game-activity_INCLUDE_DIRECTORIES}/game-activity/GameActivity.h)
target_link_libraries(${SIERRA_APPLICATION_NAME} PUBLIC game-activity::game-activity_static)

# Link libraries
target_link_libraries(Sierra PUBLIC
    log
    android
    game-activity::game-activity_static
)

# Add include directories
target_include_directories(Sierra PUBLIC
    ${CMAKE_ANDROID_NDK}/sources
    ${game-activity_INCLUDE_DIRECTORIES}
)
