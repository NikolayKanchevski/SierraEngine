target_sources(Sierra PRIVATE
    FoundationFileManager.mm
    FoundationFileManager.h
    FoundationFileStream.mm
    FoundationFileStream.h
    NSFileErrorHandler.mm
    NSFileErrorHandler.h
)

target_link_libraries(Sierra PRIVATE "-framework Foundation")
