include(DeviceInfo.cmake)

macro(LINK_STATIC_LIBRARY TARGET TYPE LIBRARY_FILE_PATH)
    target_link_libraries(${TARGET} ${TYPE} ${LIBRARY_FILE_PATH})
endmacro()

macro(LINK_DYNAMIC_LIBRARY TARGET TYPE LIBRARY_FILE_PATH)
    if(DEVICE_OS STREQUAL "macOS")
        file(COPY ${CMAKE_CURRENT_LIST_DIR}/${LIBRARY_FILE_PATH} DESTINATION ${CMAKE_BINARY_DIR}/DLLs/)
        get_filename_component(LIBRARY_NAME ${LIBRARY_FILE_PATH} NAME)
        target_link_libraries(${TARGET} ${TYPE} ${CMAKE_BINARY_DIR}/DLLs/${LIBRARY_NAME})
    else()
        file(COPY ${CMAKE_CURRENT_LIST_DIR}/${LIBRARY_FILE_PATH} DESTINATION ${CMAKE_BINARY_DIR}/)
        target_link_libraries(${TARGET} ${TYPE} ${CMAKE_CURRENT_LIST_DIR}/${LIBRARY_FILE_PATH}.lib)

    endif()
endmacro()