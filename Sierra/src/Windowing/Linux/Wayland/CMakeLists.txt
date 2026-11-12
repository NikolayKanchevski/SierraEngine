target_sources(Sierra PRIVATE
    WaylandContext.cpp
    WaylandContext.h
    WaylandCursorManager.cpp
    WaylandCursorManager.h
    WaylandInputManager.cpp
    WaylandInputManager.h
    WaylandScreen.cpp
    WaylandScreen.h
    WaylandWindow.cpp
    WaylandWindow.h
    WaylandWindowManager.cpp
    WaylandWindowManager.h
)

include(CheckIncludeFiles)
include(CheckFunctionExists)
check_function_exists(memfd_create HAVE_MEMFD_CREATE)
if (HAVE_MEMFD_CREATE)
    target_compile_definitions(Sierra PRIVATE HAVE_MEMFD_CREATE)
endif()

enable_language(C)

find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
if (NOT WAYLAND_SCANNER_EXECUTABLE)
    message(FATAL_ERROR "Failed to find wayland-scanner")
endif()

macro(generate_wayland_protocol protocol_file)
    set(protocol_path "${CMAKE_CURRENT_LIST_DIR}/protocols/${protocol_file}")

    string(REGEX REPLACE "\\.xml$" "-client-protocol.h"      header_name "${protocol_file}")
    string(REGEX REPLACE "\\.xml$" "-client-protocol-code.c" code_name   "${protocol_file}")
    set(header_file "${CMAKE_CURRENT_LIST_DIR}/protocols/${header_name}")
    set(code_file   "${CMAKE_CURRENT_LIST_DIR}/protocols/${code_name}")

    execute_process(
            COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" client-header "${protocol_path}" "${header_file}"
            COMMAND_ERROR_IS_FATAL ANY)
    target_sources(Sierra PRIVATE ${header_file})

    # Core protocol is already provided by libwayland-client, so its definitions must not be compiled in again
    if(NOT "${protocol_file}" STREQUAL "wayland.xml")
        execute_process(
                COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" private-code "${protocol_path}" "${code_file}"
                COMMAND_ERROR_IS_FATAL ANY)

        target_sources(Sierra PRIVATE ${code_file})
    endif()
endmacro()

generate_wayland_protocol("wayland.xml")
generate_wayland_protocol("viewporter.xml")
generate_wayland_protocol("xdg-shell.xml")
generate_wayland_protocol("pointer-constraints-unstable-v1.xml")
generate_wayland_protocol("relative-pointer-unstable-v1.xml")
generate_wayland_protocol("fractional-scale-v1.xml")
generate_wayland_protocol("tablet-unstable-v2.xml")
generate_wayland_protocol("cursor-shape-v1.xml")
generate_wayland_protocol("xdg-activation-v1.xml")
generate_wayland_protocol("xdg-decoration-unstable-v1.xml")

include(FindPkgConfig)

pkg_check_modules(Wayland REQUIRED
    wayland-client>=0.2.7
    wayland-cursor>=0.2.7
    xkbcommon>=0.5.0
)

target_link_directories(Sierra PRIVATE ${Wayland_LIBRARY_DIRS})
target_link_libraries(Sierra PRIVATE ${Wayland_LIBRARIES})

target_include_directories(Sierra SYSTEM PRIVATE ${Wayland_INCLUDE_DIRS})
target_include_directories(Sierra SYSTEM PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/protocols/)

if (NOT CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(EpollShim)
    if (EPOLLSHIM_FOUND)
        target_include_directories(Sierra PRIVATE ${EPOLLSHIM_INCLUDE_DIRS})
        target_link_libraries(Sierra PRIVATE ${EPOLLSHIM_LIBRARIES})
    endif()
endif()

pkg_check_modules(Libdecor REQUIRED IMPORTED_TARGET libdecor-0)
if (NOT ${Libdecor_FOUND})
    message(FATAL_ERROR "[Sierra]: Could not find Libdecor Wayland library!")
endif()

target_link_libraries(Sierra PRIVATE PkgConfig::Libdecor)