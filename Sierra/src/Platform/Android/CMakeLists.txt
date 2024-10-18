target_sources(Sierra PRIVATE
    AndroidContext.cpp
    AndroidContext.h
    GameKitContext.cpp
    GameKitContext.h
)

set(ANDROID_INCLUDE_DIRECTORY_PATH ${CMAKE_ANDROID_NDK}/sources)
target_include_directories(Sierra PUBLIC ${ANDROID_INCLUDE_DIRECTORY_PATH})

set(GAME_SDK_DIRECTORY_PATH ../../../vendor/android-game-sdk)

set(GAME_ACTIVITY_DIRECTORY_PATH ${GAME_SDK_DIRECTORY_PATH}/game-activity/)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u \ Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")
add_subdirectory(${GAME_ACTIVITY_DIRECTORY_PATH} ${GAME_ACTIVITY_DIRECTORY_PATH})
target_include_directories(Sierra PUBLIC ${GAME_ACTIVITY_DIRECTORY_PATH}/prefab-src/modules/game-activity/include/)

target_link_libraries(Sierra PRIVATE game-activity_static)

# NOTE: We link shared library to application, just to register the entry point
target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE game-activity)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.cpp)
target_link_libraries(Sierra PUBLIC
    log
    android
    game-activity_static
)

