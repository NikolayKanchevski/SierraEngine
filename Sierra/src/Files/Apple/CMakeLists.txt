target_sources(Sierra PRIVATE
    FoundationFileManager.mm
    FoundationFileManager.h
    FoundationFileStream.mm
    FoundationFileStream.h
    FoundationFileUtilities.mm
    FoundationFileUtilities.h
)

target_link_libraries(Sierra PRIVATE "-framework Foundation")
