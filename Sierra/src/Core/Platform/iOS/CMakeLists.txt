# Link source files
target_sources(Sierra PRIVATE
    iOSInstance.mm
    iOSInstance.h
    UIKitContext.mm
    UIKitContext.h
    UIKitScreen.mm
    UIKitScreen.h
    UIKitSelectorBridge.mm
    UIKitSelectorBridge.h
    UIKitTemporaryCreateInfoStorage.mm
    UIKitTemporaryCreateInfoStorage.h
    UIKitTouchManager.mm
    UIKitTouchManager.h
    UIKitWindow.mm
    UIKitWindow.h
)

# Link libraries
target_link_libraries(Sierra PRIVATE
    "-framework UIKit"
    "-framework QuartzCore"
)