# Link source files
target_sources(Sierra PRIVATE
    AndroidInstance.h
    AndroidInstance.cpp
)
target_sources(Sierra PUBLIC AndroidApplication.h)

# Find game activity
find_package(game-activity REQUIRED CONFIG)
get_target_property(game-activity-include game-activity::game-activity INTERFACE_INCLUDE_DIRECTORIES)

# Enable native activity
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u \ Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u AGameActivity_onCreate")
# Build app_glue
set(APP_GLUE_DIRECTORY ${game-activity-include}/game-activity/native_app_glue)
target_sources(${SIERRA_APPLICATION_NAME} PUBLIC android_main.cpp ${APP_GLUE_DIRECTORY}/android_native_app_glue.h ${APP_GLUE_DIRECTORY}/android_native_app_glue.c ${game-activity-include}/game-activity/GameActivity.h ${game-activity-include}/game-activity/GameActivity.cpp)
target_link_libraries(${SIERRA_APPLICATION_NAME} PUBLIC game-activity::game-activity_static)

# Link libraries
target_link_libraries(Sierra PUBLIC
    log
    android

)

# Add include directories
target_include_directories(Sierra PUBLIC
    ${CMAKE_ANDROID_NDK}/sources
    ${game-activity-include}
)
