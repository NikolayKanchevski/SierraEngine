target_sources(SierraEngine PRIVATE
    ArenaAllocator.h
    ArenaAllocator.cpp
    Mesh.cpp
    Mesh.h
    Surface.cpp
    Surface.h
    Vertex.h
)

add_subdirectory(Renderers)