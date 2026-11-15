target_sources(Sierra PRIVATE
    CocoaCursorManager.mm
    CocoaCursorManager.h
    CocoaInputManager.mm
    CocoaInputManager.h
    CocoaScreen.mm
    CocoaScreen.h
    CocoaWindow.mm
    CocoaWindow.h
    CocoaWindowManager.mm
    CocoaWindowManager.h
)

target_link_libraries(Sierra PUBLIC
    "-framework Cocoa"
    "-framework QuartzCore"
    "-framework UniformTypeIdentifiers"
)