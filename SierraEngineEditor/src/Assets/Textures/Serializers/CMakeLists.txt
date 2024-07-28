set(RAPID_YAML_DIRECTORY_PATH ../../../../vendor/rapidyaml)
add_subdirectory(${RAPID_YAML_DIRECTORY_PATH} ${RAPID_YAML_DIRECTORY_PATH})
target_link_libraries(SierraEngineEditor PRIVATE ryml)
target_include_directories(SierraEngineEditor PRIVATE ${RAPID_YAML_DIRECTORY_PATH}/src)
target_include_directories(SierraEngineEditor PRIVATE ${RAPID_YAML_DIRECTORY_PATH}/ext/c4core/src)

target_sources(SierraEngineEditor PRIVATE
    YAMLTextureSerializer.cpp
    YAMLTextureSerializer.h
)