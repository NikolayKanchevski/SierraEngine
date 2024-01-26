function(BuildLinuxExecutable)
    # Have a folder for all temporary resources
    set(RESOURCES_FOLDER_PATH "${CMAKE_CURRENT_BINARY_DIR}/Resources")

    # Copy icon to resources folder
    set(ICON_OUTPUT_PATH "${RESOURCES_FOLDER_PATH}/${SIERRA_APPLICATION_NAME}Icon.ico")
    configure_file(${SIERRA_APPLICATION_ICON_ICO} ${ICON_OUTPUT_PATH} COPYONLY)

    # Define desktop file data
    set(DESKTOP_FILE_DATA "
        [Desktop Entry]
        Encoding=UTF-8
        Version=${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}
        Type=Application
        Terminal=false
        Exec=${CMAKE_CURRENT_BINARY_DIR}/${SIERRA_APPLICATION_NAME}
        Name=${SIERRA_APPLICATION_NAME}
        Icon=${ICON_OUTPUT_PATH}
    ")

    # Create and write to desktop file
    set(DESKTOP_FILE_PATH "$ENV{HOME}/.local/share/applications/${SIERRA_APPLICATION_NAME}.desktop")
    file(WRITE ${DESKTOP_FILE_PATH} ${DESKTOP_FILE_DATA})

    # Create executable
    add_executable(${SIERRA_APPLICATION_NAME} ${SOURCE_FILES})

    # Copy resources to resources folder
    add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/resources" "${CMAKE_BINARY_DIR}/Resources")
endfunction()