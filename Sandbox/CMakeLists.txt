# === PROJECT & CMAKE SETTINGS === #
cmake_minimum_required(VERSION 3.20)
project(Sandbox)
set(CMAKE_CXX_STANDARD 20)

# === EXECUTABLE GENERATION === #
set(SOURCE_FILES src/main.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# === LINKING THE ENGINE === #
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(SIERRA_BUILD_SHARED_LIBRARY ON)
else()
    set(SIERRA_BUILD_STATIC_LIBRARY ON)
endif()

add_subdirectory(../Sierra ${CMAKE_CURRENT_BINARY_DIR}/Sierra)
target_link_libraries(${PROJECT_NAME} PRIVATE Sierra)