target_sources(Sierra PRIVATE
    CocoaContext.mm
    CocoaContext.h
    macOSContext.mm
    macOSContext.h
)

target_compile_options(Sierra PRIVATE
    $<$<COMPILE_LANGUAGE:OBJC>:-fno-objc-arc>
    $<$<COMPILE_LANGUAGE:OBJCXX>:-fno-objc-arc>
)

target_link_libraries(Sierra PRIVATE
    "-framework Cocoa"
    "-framework AppKit"
    "-framework UniformTypeIdentifiers"
)

# Link entry point
target_sources(${SIERRA_APPLICATION_NAME} PRIVATE main.mm)
target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE "-framework Cocoa")