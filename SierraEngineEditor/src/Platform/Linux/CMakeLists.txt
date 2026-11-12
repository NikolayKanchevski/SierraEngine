target_sources(SierraEngineEditor PRIVATE
    LinuxEditorPlatformContext.cpp
    LinuxEditorPlatformContext.h
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
if(NOT ${Qt6_FOUND})
    message(FATAL_ERROR "[Sierra Engine Editor]: Could not find Qt library!")
endif()

target_link_libraries(SierraEngineEditor PRIVATE Qt6::Widgets)