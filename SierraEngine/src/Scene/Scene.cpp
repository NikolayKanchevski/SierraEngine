//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Scene.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Scene::Scene(const SceneCreateInfo &createInfo)
        : arenaAllocator({ .renderingContext = createInfo.renderingContext }), resourceTable(createInfo.renderingContext.CreateResourceTable({ .name = "Scene Resource Table" }))
    {

    }

    /* --- POLLING METHODS --- */

    Entity Scene::CreateEntity(const std::string_view name)
    {
        return Entity(name, registry.create(), &registry);
    }

    void Scene::DestroyEntity(const Entity entity)
    {
        registry.destroy(entity.entity);
    }

    void Scene::Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer)
    {

    }

}