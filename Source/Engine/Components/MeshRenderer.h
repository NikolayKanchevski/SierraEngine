//
// Created by Nikolay Kanchevski on some date lul.
//

#pragma once

#include "Component.h"

#include "../Classes/Mesh.h"
#include "../Classes/Binary.h"
#include "../Classes/Material.h"
#include "../Classes/IdentifierPool.h"
#include "../../Core/Rendering/RenderingSettings.h"
#include "../../Core/Rendering/Abstractions/Texture.h"

namespace Sierra::Engine
{

    class MeshRenderer : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        MeshRenderer(UniquePtr<Mesh> &givenMesh, SharedPtr<Material> &material);
        void OnAddComponent() override;

        /* --- POLLING METHODS --- */
        void OnDrawUI() override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline SharedPtr<Material>& GetMaterial() { return material; }
        [[nodiscard]] inline UniquePtr<Mesh>& GetMesh() { return mesh; }

        [[nodiscard]] inline Matrix4x4 GetModelMatrix() const { return modelMatrix; };
        [[nodiscard]] inline Matrix3x3 GetNormalMatrix() const { return normalMatrix; };

        [[nodiscard]] inline uint32 GetMeshID() const { return meshID; }
        [[nodiscard]] inline Binary GetTexturePresence() const { return texturePresence; };

        /* --- SETTER METHODS --- */
        void Destroy() override;

    private:
        UniquePtr<Mesh>& mesh;
        SharedPtr<Material>& material;

        Matrix4x4 modelMatrix;
        Matrix3x3 normalMatrix;

        Binary texturePresence = 0; // A binary string of encoded bools, which indicate wether a given texture type is present

        uint32 meshID = 0;
        inline static auto IDPool = IdentifierPool<uint32>(MAX_MESHES);
    };

}