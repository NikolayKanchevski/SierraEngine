if(NOT WIN32 AND NOT UNIX)
    message(FATAL_ERROR "Error: Android toolchain can only be used on Windows and Linux!")
endif()

set(SIERRA_BUILD_ANDROID_STUDIO_PROJECT ON)
function(BuildAndroidStudioProject)
    find_package(Python)
    if(NOT Python_FOUND)
        message(FATAL_ERROR "[Sierra]: Cannot build Android Studio, because Python was not found! Did you install it form https://www.python.org/downloads/?")
    endif()

    add_custom_target(AndroidStudio ALL)
    set(CONFIG_DIRECTORY_PATH "${SIERRA_DIRECTORY_PATH}/src/Core/Platform/Android/config")
    set(ANDROID_DIRECTORY_PATH "${PROJECT_SOURCE_DIR}/.android")

    set(KOTLIN_DIRECTORY_PATH "${ANDROID_DIRECTORY_PATH}/kotlin/com/sierra/${SIERRA_APPLICATION_NAME}")
    add_custom_command(TARGET AndroidStudio COMMAND ${Python_EXECUTABLE} "${SIERRA_DIRECTORY_PATH}/scripts/FileGenerator.py" "${PROJECT_SOURCE_DIR}/build.gradle.kts" "${CONFIG_DIRECTORY_PATH}/build.gradle.kts" "'APPLICATION_NAME=${SIERRA_APPLICATION_NAME},APPLICATION_VERSION_NAME=${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}'")
    add_custom_command(TARGET AndroidStudio COMMAND ${Python_EXECUTABLE} "${SIERRA_DIRECTORY_PATH}/scripts/FileGenerator.py" "${KOTLIN_DIRECTORY_PATH}/MainActivity.kt" "${CONFIG_DIRECTORY_PATH}/MainActivity.kt" "'APPLICATION_NAME=${SIERRA_APPLICATION_NAME}'")
    add_custom_command(TARGET AndroidStudio COMMAND ${Python_EXECUTABLE} "${SIERRA_DIRECTORY_PATH}/scripts/FileGenerator.py" "${ANDROID_DIRECTORY_PATH}/AndroidManifest.xml" "${CONFIG_DIRECTORY_PATH}/AndroidManifest.xml" "'NONE=NONE'")

    set(RES_DIRECTORY_PATH "${ANDROID_DIRECTORY_PATH}/res")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E remove_directory ${RES_DIRECTORY_PATH})
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory ${RES_DIRECTORY_PATH})

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY_PATH}/mipmap-hdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY_PATH}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY_PATH}/mipmap-hdpi/ic_launcher.png")

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY_PATH}/mipmap-mdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY_PATH}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY_PATH}/mipmap-mdpi/ic_launcher.png")

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY_PATH}/mipmap-xhdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY_PATH}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY_PATH}/mipmap-xhdpi/ic_launcher.png")

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY_PATH}/mipmap-xxhdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY_PATH}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY_PATH}/mipmap-xxhdpi/ic_launcher.png")
endfunction()