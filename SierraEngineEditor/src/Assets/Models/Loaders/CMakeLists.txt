set(FASTGLTF_DIRECTORY_PATH ../../../../vendor/fastgltf)
set(FASTGLTF_COMPILE_AS_CPP20 ON)
set(FASTGLTF_USE_STD_MODULE ON)
add_subdirectory(${FASTGLTF_DIRECTORY_PATH} ${FASTGLTF_DIRECTORY_PATH})
target_link_libraries(SierraEngineEditor PRIVATE fastgltf)
target_include_directories(SierraEngineEditor PRIVATE ${FASTGLTF_DIRECTORY_PATH}/include)

target_sources(SierraEngineEditor PRIVATE
    AutoModelLoader.cpp
    AutoModelLoader.h
    glTFModelLoader.cpp
    glTFModelLoader.h
)