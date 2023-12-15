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
    set(CONFIG_DIRECTORY "${SIERRA_DIRECTORY}/src/Core/Platform/Android/config")
    set(ANDROID_DIRECTORY "${PROJECT_SOURCE_DIR}/.android")

    set(KOTLIN_DIRECTORY "${ANDROID_DIRECTORY}/kotlin/com/sierra/${SIERRA_APPLICATION_NAME}")
    add_custom_command(TARGET AndroidStudio COMMAND ${Python_EXECUTABLE} "${SIERRA_DIRECTORY}/scripts/FileGenerator.py" "${PROJECT_SOURCE_DIR}/build.gradle.kts" "${CONFIG_DIRECTORY}/build.gradle.kts" "'APPLICATION_NAME=${SIERRA_APPLICATION_NAME},APPLICATION_VERSION_NAME=${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}'")
    add_custom_command(TARGET AndroidStudio COMMAND ${Python_EXECUTABLE} "${SIERRA_DIRECTORY}/scripts/FileGenerator.py" "${KOTLIN_DIRECTORY}/MainActivity.kt" "${CONFIG_DIRECTORY}/MainActivity.kt" "'APPLICATION_NAME=${SIERRA_APPLICATION_NAME}'")
    add_custom_command(TARGET AndroidStudio COMMAND ${Python_EXECUTABLE} "${SIERRA_DIRECTORY}/scripts/FileGenerator.py" "${ANDROID_DIRECTORY}/AndroidManifest.xml" "${CONFIG_DIRECTORY}/AndroidManifest.xml" "'NONE=NONE'")

    set(RES_DIRECTORY "${ANDROID_DIRECTORY}/res")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E remove_directory ${RES_DIRECTORY})
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory ${RES_DIRECTORY})

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY}/mipmap-hdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY}/mipmap-hdpi/ic_launcher.png")

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY}/mipmap-mdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY}/mipmap-mdpi/ic_launcher.png")

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY}/mipmap-xhdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY}/mipmap-xhdpi/ic_launcher.png")

    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E make_directory "${RES_DIRECTORY}/mipmap-xxhdpi")
    add_custom_command(TARGET AndroidStudio COMMAND ${CMAKE_COMMAND} -E copy "${SIERRA_DIRECTORY}/${SIERRA_APPLICATION_ICON_PNG}" "${RES_DIRECTORY}/mipmap-xxhdpi/ic_launcher.png")
endfunction()