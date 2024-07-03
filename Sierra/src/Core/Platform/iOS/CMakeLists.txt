enable_language(OBJCXX)
target_sources(Sierra PRIVATE
    iOSContext.mm
    iOSContext.h
    iOSWindowManager.mm
    iOSWindowManager.h
    UIKitContext.mm
    UIKitContext.h
    UIKitScreen.mm
    UIKitScreen.h
    UIKitTouchManager.mm
    UIKitTouchManager.h
    UIKitWindow.mm
    UIKitWindow.h
)

target_link_libraries(Sierra PRIVATE
    "-framework Foundation"
    "-framework UIKit"
    "-framework QuartzCore"
)

# Disable automatic reference counting
set_xcode_property(Sierra CLANG_ENABLE_OBJC_ARC "NO" "All")
set_xcode_property(Sierra GCC_WARN_INHIBIT_ALL_WARNINGS "YES" "All")

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.mm)
target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE "-framework UIKit")