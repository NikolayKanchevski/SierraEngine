target_sources(Sierra PRIVATE
    CocoaCursorManager.mm
    CocoaCursorManager.h
    CocoaInputManager.mm
    CocoaInputManager.h
    CocoaScreen.mm
    CocoaScreen.h
    CocoaWindow.mm
    CocoaWindow.h
)

target_link_libraries(Sierra PRIVATE
    "-framework Cocoa"
    "-framework QuartzCore"
)