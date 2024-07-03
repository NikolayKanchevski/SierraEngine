enable_language(OBJCXX)
target_sources(Sierra PRIVATE
    FoundationFileManager.mm
    FoundationFileManager.h
)

target_link_libraries(Sierra PRIVATE
    "-framework Foundation"
)