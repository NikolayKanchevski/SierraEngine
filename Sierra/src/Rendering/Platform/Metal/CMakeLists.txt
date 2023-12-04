# Set Metal's prerequisites (Metal is guaranteed to be found)
set(Metal_LIBRARIES
    "-framework Metal"
    "-framework QuartzCore"
    "-framework Foundation"
)
message(STATUS "[Sierra]: Building Metal...")

# Link Metal
set(METAL_CPP_DIRECTORY ../../../../vendor/metal-cpp)
target_link_libraries(Sierra PRIVATE ${Metal_LIBRARIES})
target_include_directories(Sierra PRIVATE ${METAL_CPP_DIRECTORY})

# Set Metal definitions
target_compile_definitions(Sierra PUBLIC "SR_METAL_SUPPORTED")

# Add Metal source files
target_sources(Sierra PRIVATE
    MetalContext.cpp
    MetalContext.h
    MetalDevice.cpp
    MetalDevice.h
    MetalResource.cpp
    MetalResource.h
)