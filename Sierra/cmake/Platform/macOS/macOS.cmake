enable_language(OBJCXX)
function(BuildMacOSExecutable SOURCE_FILES)
    # Create executable and set its properties
    add_executable(${SIERRA_APPLICATION_NAME} MACOSX_BUNDLE ${SOURCE_FILES})
    set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES
        BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME ${SIERRA_APPLICATION_NAME}
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}"
        MACOSX_BUNDLE_PRODUCT_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${SIERRA_APPLICATION_VERSION_MAJOR}.${SIERRA_APPLICATION_VERSION_MINOR}.${SIERRA_APPLICATION_VERSION_PATCH}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${SIERRA_APPLICATION_VERSION_MAJOR}"
    )
    set_target_properties(Sierra PROPERTIES
        COMPILE_FLAGS "-fno-objc-arc"
    )

    # Copy resources to bundle
    set(RESOURCES_DIRECTORY "${CMAKE_SOURCE_DIR}/resources")
    file(GLOB RESOURCES_DIRECTORY_ITEMS "${RESOURCES_DIRECTORY}/*.*")
    list(LENGTH RESOURCES_DIRECTORY_ITEMS RESOURCES_DIRECTORY_ITEMS_LENGTH)
    if(RESOURCES_DIRECTORY_ITEMS_LENGTH GREATER 0)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCES_DIRECTORY}" "${CMAKE_BINARY_DIR}/${SIERRA_APPLICATION_NAME}.app/Contents/Resources")
    endif()

    # Link icon with the application
    set_source_files_properties(${SIERRA_APPLICATION_ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(${SIERRA_APPLICATION_NAME} PRIVATE ${SIERRA_APPLICATION_ICON_ICNS})
endfunction()