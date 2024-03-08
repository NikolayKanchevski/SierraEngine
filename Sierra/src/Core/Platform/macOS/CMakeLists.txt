target_sources(Sierra PRIVATE
    CocoaContext.mm
    CocoaContext.h
    CocoaCursorManager.mm
    CocoaCursorManager.h
    CocoaInputManager.mm
    CocoaInputManager.h
    CocoaScreen.mm
    CocoaScreen.h
    CocoaWindow.mm
    CocoaWindow.h
    macOSContext.mm
    macOSContext.h
)

# Link platform-specific libraries
target_link_libraries(Sierra PRIVATE
    "-framework Cocoa"
    "-framework QuartzCore"
    "-framework Foundation"
)