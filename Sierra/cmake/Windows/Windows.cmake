function(BuildWindowsExecutable SOURCE_FILES)
    # Have a folder for all temporary resources
    set(RESOURCES_DIRECTORY_PATH "${CMAKE_CURRENT_BINARY_DIR}/Resources")

    # Copy icon to resources folder
    set(ICON_OUTPUT_PATH "${RESOURCES_DIRECTORY_PATH}/${SIERRA_APPLICATION_NAME}Icon.ico")
    configure_file(${SIERRA_APPLICATION_ICON_ICO} ${ICON_OUTPUT_PATH} COPYONLY)

    # Define RC file data
    # NOTE: Icon is loaded by Win32Context under the name IDR_MAINFRAME, so it must not be renamed
    set(RC_FILE_DATA "
        IDR_MAINFRAME ICON
        \"${SIERRA_APPLICATION_NAME}Icon.ico\"
    ")

    # Embed application manifest (MSVC's linker embeds it itself, when listed as a source, while other toolchains need it as a resource of type RT_MANIFEST)
    set(MANIFEST_FILE_PATH "${SIERRA_DIRECTORY_PATH}/cmake/Windows/Application.manifest")
    set(MANIFEST_SOURCE_FILE "")
    if(MSVC)
        set(MANIFEST_SOURCE_FILE ${MANIFEST_FILE_PATH})
    else()
        string(APPEND RC_FILE_DATA "
        1 24 \"${MANIFEST_FILE_PATH}\"
        ")
    endif()

    # Create and write to RC file
    set(RC_FILE_PATH "${RESOURCES_DIRECTORY_PATH}/Application.rc")
    file(WRITE ${RC_FILE_PATH} ${RC_FILE_DATA})

    # Create executable
    add_executable(${SIERRA_APPLICATION_NAME} WIN32 ${SOURCE_FILES} ${RC_FILE_PATH} ${MANIFEST_SOURCE_FILE})

    # Copy resources to resources folder
    set(RESOURCES_DIRECTORY_PATH "${CMAKE_SOURCE_DIR}/resources")
    file(GLOB_RECURSE RESOURCES_DIRECTORY_ITEMS "${RESOURCES_DIRECTORY_PATH}/*.*")
    list(LENGTH RESOURCES_DIRECTORY_ITEMS RESOURCES_DIRECTORY_ITEMS_LENGTH)
    if(RESOURCES_DIRECTORY_ITEMS_LENGTH GREATER 0)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCES_DIRECTORY_PATH}" "${CMAKE_BINARY_DIR}/Resources")
    endif()
endfunction()
