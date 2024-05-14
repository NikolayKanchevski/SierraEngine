//
// Created by Nikolay Kanchevski on 22.04.24.
//

#pragma once

namespace SierraEngine
{

    struct MeshCreateInfo
    {
        uint64 vertexByteOffset = 0;
        uint32 vertexCount = 0;

        uint64 indexByteOffset = 0;
        uint32 indexCount = 0;
    };

    class SIERRA_ENGINE_API Mesh final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Mesh(const MeshCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint64 GetVertexByteOffset() const { return vertexByteOffset; }
        [[nodiscard]] inline uint64 GetIndexByteOffset() const { return indexByteOffset; }

        [[nodiscard]] inline uint32 GetVertexCount() const { return vertexCount; }
        [[nodiscard]] inline uint32 GetIndexCount() const { return indexCount; }

        /* --- OPERATORS --- */
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        /* --- MOVE SEMANTICS --- */
        Mesh(Mesh&& other) = default;
        Mesh& operator=(Mesh&& other) = delete;

        /* --- DESTRUCTOR --- */
        ~Mesh() = default;

    private:
        const uint64 vertexByteOffset = 0;
        const uint64 indexByteOffset = 0;

        const uint32 vertexCount = 0;
        const uint32 indexCount = 0;


    };

}
