target_sources(Sierra PRIVATE
    LinuxContext.cpp
    LinuxContext.h
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(NOT ${Qt6_FOUND})
    message(FATAL_ERROR "[Sierra]: Could not find Qt library!")
endif()
target_link_libraries(Sierra PRIVATE Qt6::Widgets)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0)
target_link_libraries(Sierra PRIVATE PkgConfig::GIO)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.cpp)