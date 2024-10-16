target_sources(Sierra PRIVATE
    CocoaContext.mm
    CocoaContext.h
    macOSContext.mm
    macOSContext.h
)

target_link_libraries(Sierra PRIVATE
    "-framework Cocoa"
    "-framework AppKit"
)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.mm)
target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE "-framework Cocoa")