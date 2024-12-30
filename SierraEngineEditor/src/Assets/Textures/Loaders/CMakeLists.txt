set(STB_DIRECTORY_PATH ../../../../vendor/stb)
set(STB_SOURCE_FILES
    ${STB_DIRECTORY_PATH}/stb_image.h
)
target_sources(SierraEngineEditor PRIVATE ${STB_SOURCE_FILES})
target_include_directories(SierraEngineEditor PRIVATE ${STB_DIRECTORY_PATH})

target_sources(SierraEngineEditor PRIVATE
    AutoImageLoader.cpp
    AutoImageLoader.h
    STBImageLoader.cpp
    STBImageLoader.h
)