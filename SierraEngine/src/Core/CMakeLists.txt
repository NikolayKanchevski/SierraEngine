set(ENTT_DIRECTORY_PATH ../../vendor/entt)
add_subdirectory(${ENTT_DIRECTORY_PATH} ${ENTT_DIRECTORY_PATH})
target_link_libraries(SierraEngine PRIVATE EnTT)
target_include_directories(SierraEngine PRIVATE ${ENTT_DIRECTORY_PATH}/single_include/)

target_sources(SierraEngine PRIVATE
    Application.cpp
    Application.h
    Component.h
    ResourcePool.hpp
    Scene.cpp
    Scene.h
    ThreadPool.hpp
)

add_subdirectory(Components)