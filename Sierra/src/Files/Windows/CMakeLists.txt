target_sources(Sierra PRIVATE
    Win32FileManager.cpp
    Win32FileManager.h
    Win32FileStream.cpp
    Win32FileStream.h
)

# Link system libraries
target_link_libraries(Sierra PRIVATE
    shell32
    ole32
    uuid
)
