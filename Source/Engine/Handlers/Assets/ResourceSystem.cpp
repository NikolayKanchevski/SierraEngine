//
// Created by Nikolay Kanchevski on 6.08.23.
//

#include "ResourceSystem.h"

#include "../Project.h"
#include "AssetImporter.h"
#include "EmbeddedData/EmbeddedTextures.h"
#include "EmbeddedData/EmbeddedModels.h"

namespace Sierra::Engine
{

    #pragma region Texture Collection

        /* --- POLLING METHODS --- */

        bool TextureCollection::Load(const AssetID &ID, SharedPtr<Rendering::Texture> &texture)
        {
            texture = TextureImporter().Import(ID);
            return texture != nullptr;
        }

        /* --- DESTRUCTOR --- */

        void TextureCollection::Destroy()
        {
            for (const auto &[ID, resourceData] : resourceMap)
            {
                resourceData.resource->Destroy();
            }
            ResourceSystem::Destroy();
        }

    #pragma endregion

    #pragma region Cubemap Collection

        /* --- POLLING METHODS --- */

        bool CubemapCollection::Load(const AssetID &ID, SharedPtr<Rendering::Cubemap> &cubemap)
        {
            cubemap = CubemapImporter().Import(ID);
            return cubemap != nullptr;
        }

        /* --- DESTRUCTOR --- */

        void CubemapCollection::Destroy()
        {
            for (const auto &[ID, resourceData] : resourceMap)
            {
                resourceData.resource->Destroy();
            }
            ResourceSystem::Destroy();
        }

    #pragma endregion

    #pragma region Material Collection

        /* --- POLLING METHODS --- */

        bool MaterialCollection::Load(const AssetID &ID, SharedPtr<Material> &material)
        {
            material = MaterialImporter().Import(ID);
            return material != nullptr;
        }

        /* --- DESTRUCTOR --- */

        void MaterialCollection::Destroy()
        {
            ResourceSystem::Destroy();
        }

    #pragma endregion

    #pragma region Model Collection

        /* --- POLLING METHODS --- */

        bool ModelCollection::Load(const AssetID &ID, SharedPtr<Model> &model)
        {
            model = ModelImporter().Import(ID);
            return model != nullptr;
        }

        /* --- DESTRUCTOR --- */

        void ModelCollection::Destroy()
        {
            for (const auto &[ID, resourceData] : resourceMap)
            {
                resourceData.resource->Destroy();
            }
            ResourceSystem::Destroy();
        }

    #pragma endregion

    #pragma region Defaul Collection

        /* --- CONSTRUCTOR --- */

        DefaultCollection::DefaultCollection()
        {
            using namespace Rendering;

            // Create default diffuse texture
            DEFAULT_DIFFUSE_TEXTURE_DATA_IMPLEMENTATION;
            defaultTextures[TextureType::DIFFUSE] = Texture::Load({
                .data = DEFAULT_DIFFUSE_TEXTURE_DATA,
                .width = DEFAULT_DIFFUSE_TEXTURE_WIDTH,
                .height = DEFAULT_DIFFUSE_TEXTURE_HEIGHT,
                .channels = Rendering::ImageChannels::RGB,
                .memoryType = Rendering::ImageMemoryType::UINT8_NORM,
                .enableSmoothFiltering = false
            });

            // Create default specular texture
            DEFAULT_SPECULAR_TEXTURE_DATA_IMPLEMENTATION;
            defaultTextures[TextureType::SPECULAR] = Texture::Load({
                .data = DEFAULT_SPECULAR_TEXTURE_DATA,
                .width = DEFAULT_SPECULAR_TEXTURE_WIDTH,
                .height = DEFAULT_SPECULAR_TEXTURE_HEIGHT,
                .channels = Rendering::ImageChannels::R,
                .memoryType = Rendering::ImageMemoryType::UINT8_NORM,
                .enableSmoothFiltering = false
            });

            // Create default normal texture
            DEFAULT_NORMAL_TEXTURE_DATA_IMPLEMENTATION;
            defaultTextures[TextureType::NORMAL] = Texture::Load({
                .data = DEFAULT_NORMAL_TEXTURE_DATA,
                .width = DEFAULT_NORMAL_TEXTURE_WIDTH,
                .height = DEFAULT_NORMAL_TEXTURE_HEIGHT,
                .channels = Rendering::ImageChannels::RGB,
                .memoryType = Rendering::ImageMemoryType::UINT8_NORM,
                .enableSmoothFiltering = false
            });

            // Create default height map
            DEFAULT_HEIGHT_TEXTURE_DATA_IMPLEMENTATION;
            defaultTextures[TextureType::HEIGHT] = Texture::Load({
                .data = DEFAULT_HEIGHT_TEXTURE_DATA,
                .width = DEFAULT_HEIGHT_TEXTURE_WIDTH,
                .height = DEFAULT_HEIGHT_TEXTURE_HEIGHT,
                .channels = Rendering::ImageChannels::R,
                .memoryType = Rendering::ImageMemoryType::UINT8_NORM,
                .enableSmoothFiltering = false
            });

            // Create default skybox
            DEFAULT_SKYBOX_DATA_IMPLEMENTATION;
            defaultCubemaps[CubemapType::SKYBOX] = Cubemap::Load({
                .data = { DEFAULT_SKYBOX_DATA_RIGHT, DEFAULT_SKYBOX_DATA_LEFT, DEFAULT_SKYBOX_DATA_TOP, DEFAULT_SKYBOX_DATA_BOTTOM, DEFAULT_SKYBOX_DATA_FRONT, DEFAULT_SKYBOX_DATA_BACK },
                .width = 512,
                .height = 512
            });

            // Create default material
            Material rawDefaultMaterial = Material({ defaultTextures[TextureType::DIFFUSE], defaultTextures[TextureType::SPECULAR], defaultTextures[TextureType::NORMAL], defaultTextures[TextureType::HEIGHT] });
            defaultMaterial = std::make_shared<Material>(rawDefaultMaterial);

            // Create default cube
            DEFAULT_CUBE_DATA_IMPLEMENTATION;
            defaultModels[ModelType::CUBE] = Model::Create({
                .rootNode = {
                    .name = "Cube",
                    .meshInfo = ModelCreateInfoMeshNode {
                        .vertexOffset = 0,
                        .vertexCount = DEFAULT_CUBE_DATA_VERTEX_COUNT,
                        .indexOffset = 0,
                        .indexCount = DEFAULT_CUBE_DATA_INDEX_COUNT,
                        .material = defaultMaterial
                    }
                },
                .meshCount = 1,
                .allVertices = DEFAULT_CUBE_ALL_VERTICES,
                .allIndices = DEFAULT_CUBE_ALL_INDICES
            });
        }

        /* --- DESTRUCTOR --- */

        void DefaultCollection::Destroy()
        {
            for (const auto &[type, cubemap] : defaultCubemaps) cubemap->Destroy();
            for (const auto &[type, texture] : defaultTextures) texture->Destroy();
            for (const auto &[type, model] : defaultModels) model->Destroy();
        }

    #pragma endregion

    #pragma region Engine Icon Collection

        static constexpr uint32 ICON_SIZE = 128;

        /* --- CONSTRUCTORS --- */

        EngineIconCollection::EngineIconCollection()
        {
            DEFAULT_ICON_ATLAS_DATA_IMPLEMENTATION;
            iconAtlas = Rendering::Texture::Load({
                .data = DEFAULT_ICON_ATLAS_DATA,
                .width = DEFAULT_ICON_ATLAS_WIDTH,
                .height = DEFAULT_ICON_ATLAS_HEIGHT,
                .memoryType = Rendering::ImageMemoryType::UINT8_NORM,
                .enableSmoothFiltering = true
            });
        }

        /* --- GETTER METHODS --- */

        ImTextureID EngineIconCollection::GetEngineIcon(const EngineIconType iconType, Vector2 &UV0, Vector2 &UV1)
        {
            // Calculate starting offset for icon
            UV0 = {
                static_cast<uint32>(iconType) % (iconAtlas->GetWidth() / ICON_SIZE) * ICON_SIZE + 1.0f,
                static_cast<uint32>(iconType) / (iconAtlas->GetWidth() / ICON_SIZE) * ICON_SIZE + 1.0f
            };

            // Calculate end offset for icon
            UV1 = {
                UV0.x + ICON_SIZE - 2.0f,
                UV0.y + ICON_SIZE - 2.0f
            };

            // Normalize UVs
            UV0.x /= iconAtlas->GetWidth();
            UV1.x /= iconAtlas->GetWidth();
            UV0.y /= iconAtlas->GetHeight();
            UV1.y /= iconAtlas->GetHeight();

            // Return icon atlas
            return iconAtlas->GetImGuiTextureID();
        }

        /* --- DESTRUCTOR --- */

        void EngineIconCollection::Destroy()
        {
            iconAtlas->Destroy();
        }

    #pragma endregion
}