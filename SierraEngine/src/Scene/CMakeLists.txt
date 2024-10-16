set(ENTT_DIRECTORY_PATH ../../vendor/entt)
add_subdirectory(${ENTT_DIRECTORY_PATH} ${ENTT_DIRECTORY_PATH})
target_link_libraries(SierraEngine PRIVATE EnTT)
target_include_directories(SierraEngine PUBLIC ${ENTT_DIRECTORY_PATH}/single_include/)

target_sources(SierraEngine PRIVATE
    Component.cpp
    Component.h
    Scene.cpp
    EntityID.h
    Scene.h
)

add_subdirectory(Components)