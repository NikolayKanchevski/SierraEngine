target_sources(Sierra PRIVATE
    UIKitScreen.mm
    UIKitScreen.h
    UIKitTouchManager.mm
    UIKitTouchManager.h
    UIKitWindow.mm
    UIKitWindow.h
)

target_link_libraries(Sierra PRIVATE
    "-framework UIKit"
    "-framework QuartzCore"
)