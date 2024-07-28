set(STBI_DIRECTORY_PATH ../../../../vendor/stb)
target_sources(SierraEngineEditor PRIVATE
    ${STBI_DIRECTORY_PATH}/stb_image.h
    ${STBI_DIRECTORY_PATH}/stb_image_resize2.h
)
target_include_directories(SierraEngineEditor PRIVATE ${STBI_DIRECTORY_PATH})

target_sources(SierraEngineEditor PRIVATE
    STBImage.cpp
    STBImage.h
)