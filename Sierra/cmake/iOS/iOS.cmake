enable_language(OBJCXX)
function(BuildIOSApplication SOURCE_FILES)
    # Validate Xcode settings
    if(NOT PLATFORM)
        message(FATAL_ERROR "[Sierra]: In order to build an Xcode project for iOS, you must compile with -DPLATFORM={TARGET_PLATFORM}!")
    endif()
    if(XCODE_VERSION LESS 14.0)
        message(FATAL_ERROR "[Sierra]: In order to build an Xcode project for iOS, your Xcode version must be 14.0 or newer!")
    endif()
    message(STATUS "[Sierra]: Building application into an Xcode project...")

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
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO
    )

    # Copy resources to bundle
    set(RESOURCES_DIRECTORY_PATH "${CMAKE_SOURCE_DIR}/resources")
    file(GLOB_RECURSE RESOURCES_DIRECTORY_ITEMS "${RESOURCES_DIRECTORY_PATH}/*.*")
    list(LENGTH RESOURCES_DIRECTORY_ITEMS RESOURCES_DIRECTORY_ITEMS_LENGTH)
    if(RESOURCES_DIRECTORY_ITEMS_LENGTH GREATER 0)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory "${RESOURCES_DIRECTORY_PATH}" $<TARGET_FILE_DIR:${SIERRA_APPLICATION_NAME}>)
    endif()

    # Link icon with the application
    set_source_files_properties(${SIERRA_APPLICATION_ICON_ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    target_sources(${SIERRA_APPLICATION_NAME} PRIVATE ${SIERRA_APPLICATION_ICON_ICNS})

    # Set Xcode project's info.plist
    set_target_properties(${SIERRA_APPLICATION_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${SIERRA_DIRECTORY_PATH}/src/Platform/iOS/config/plist.in)

    # Set up Xcode project
    function(get_all_targets var)
        set(targets)
        get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
        set(${var} ${targets} PARENT_SCOPE)
    endfunction()

    macro(get_all_targets_recursive targets dir)
        get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
        foreach(subdir ${subdirectories})
            get_all_targets_recursive(${targets} ${subdir})
        endforeach()

        get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
        list(APPEND ${targets} ${current_targets})
    endmacro()

    get_all_targets(ALL_TARGETS)
    macro(set_global_xcode_property XCODE_PROPERTY XCODE_VALUE XCODE_VARIANT)
        foreach(TARGET ${ALL_TARGETS})
            set_xcode_property(${TARGET} ${XCODE_PROPERTY} ${XCODE_VALUE} ${XCODE_VARIANT})
        endforeach()
    endmacro()

    set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "YES" "Debug")
    set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "NO" "MinSizeRel")
    set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "YES" "RelWithDebInfo")
    set_global_xcode_property(GCC_GENERATE_DEBUGGING_SYMBOLS "NO" "Release")

    set_global_xcode_property(COPY_PHASE_STRIP "NO" "Debug")
    set_global_xcode_property(COPY_PHASE_STRIP "YES" "MinSizeRel")
    set_global_xcode_property(COPY_PHASE_STRIP "NO" "RelWithDebInfo")
    set_global_xcode_property(COPY_PHASE_STRIP "YES" "Release")

    if(SIERRA_ENABLE_OPTIMIZATIONS)
        set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "0" "Debug")
        set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "s" "MinSizeRel")
        set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "3" "RelWithDebInfo")
        set_global_xcode_property(GCC_OPTIMIZATION_LEVEL "3" "Release")
    endif()

    set_global_xcode_property(IPHONEOS_DEPLOYMENT_TARGET "13.0" "All")
    set_global_xcode_property(MACOSX_DEPLOYMENT_TARGET "10.13.6" "All")
    set_xcode_property(${SIERRA_APPLICATION_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.sierra.${SIERRA_APPLICATION_NAME}" "All")
endfunction()