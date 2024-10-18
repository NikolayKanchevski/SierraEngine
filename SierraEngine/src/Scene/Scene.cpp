//
// Created by Nikolay Kanchevski on 22.04.24.
//

#include "Scene.h"

#include "Components/Tag.h"
#include "Components/Relationship.h"

namespace SierraEngine
{

    namespace
    {
        // NOTE: The point of these functions is to reserve the hash & index of 0 as null,
        // as entt uses std::numeric_limits<ENTT_ID_TYPE>::max() for its null ID

        [[nodiscard]] entt::entity EntityIDToEntity(const EntityID entityID) noexcept
        {
            return entt::entity(entityID.GetValue() - 1);
        }

        [[nodiscard]] EntityID EntityToEntityID(const entt::entity entity) noexcept
        {
            return { static_cast<ENTT_ID_TYPE>(entity) + 1 };
        }
    }

    /* --- CONSTRUCTORS --- */

    Scene::Scene(const SceneCreateInfo& createInfo)
        : name(createInfo.name)
    {
        APP_THROW_IF(createInfo.name.empty(), Sierra::InvalidValueError("Cannot create scene, as specified name must not be empty"));
    }

    /* --- POLLING METHODS --- */

    EntityID Scene::CreateEntity(const std::string_view tag) noexcept
    {
        const EntityID entityID = EntityToEntityID(registry.create());
        AddEntityComponent<Tag>(entityID, tag);
        AddEntityComponent<Relationship>(entityID);

        rootEntities.push_back(entityID);
        return entityID;
    }

    void Scene::DestroyEntity(const EntityID entityID) noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot destroy invalid entity");
            return;
        }

        RemoveEntityParent(entityID);
        for (const EntityID childID : GetEntityChildren(entityID))
        {
            RemoveEntityChild(entityID, childID);
        }

        registry.destroy(EntityIDToEntity(entityID));
    }

    /* --- SETTER METHODS --- */

    void Scene::SetEntityParent(const EntityID entityID, const EntityID parentID) noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot set parent of invalid entity");
            return;
        }

        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot assign invalid entity as parent of entity [{0}]", GetEntityComponent<Tag>(entityID)->GetName());
            return;
        }

        Relationship* const relationship = GetEntityComponent<Relationship>(entityID);
        Relationship* const newParentRelationship = GetEntityComponent<Relationship>(parentID);

        if (const EntityID currentParentID = relationship->GetParentID(); currentParentID != 0)
        {
            Relationship* const currentParentRelationship = GetEntityComponent<Relationship>(currentParentID);
            currentParentRelationship->RemoveChild(entityID);
        }

        relationship->SetParent(parentID);
        newParentRelationship->AddChild(entityID);

        const auto iterator = std::find(rootEntities.begin(), rootEntities.end(), entityID);
        if (iterator != rootEntities.end()) rootEntities.erase(iterator);
    }

    void Scene::RemoveEntityParent(const EntityID entityID) noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot remove parent of invalid entity");
            return;
        }

        Relationship* const relationship = GetEntityComponent<Relationship>(entityID);
        Relationship* const parentRelationship = GetEntityComponent<Relationship>(relationship->GetParentID());

        relationship->RemoveParent();
        parentRelationship->RemoveChild(entityID);

        rootEntities.push_back(entityID);
    }

    void Scene::AddEntityChild(const EntityID entityID, const EntityID childID) noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot add child to invalid entity");
            return;
        }

        if (!EntityExists(childID))
        {
            APP_WARNING("Cannot add invalid child to entity [{0}]", GetEntityComponent<Tag>(childID)->GetName());
            return;
        }

        Relationship* const relationship = GetEntityComponent<Relationship>(entityID);
        Relationship* const childRelationship = GetEntityComponent<Relationship>(childID);

        if (const EntityID currentParentID = childRelationship->GetParentID(); currentParentID != 0)
        {
            Relationship* const currentParentRelationship = GetEntityComponent<Relationship>(currentParentID);
            currentParentRelationship->RemoveChild(childID);
        }
        else
        {
            const auto iterator = std::find(rootEntities.begin(), rootEntities.end(), childID);
            if (iterator != rootEntities.end()) rootEntities.erase(iterator);
        }

        relationship->AddChild(childID);
        childRelationship->SetParent(entityID);
    }

    void Scene::RemoveEntityChild(const EntityID entityID, const EntityID childID) noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot remove child of invalid entity");
            return;
        }

        if (!EntityExists(childID))
        {
            APP_WARNING("Cannot remove invalid child from entity [{0}]", GetEntityComponent<Tag>(entityID)->GetTag());
            return;
        }

        Relationship* const relationship = GetEntityComponent<Relationship>(entityID);
        Relationship* const childRelationship = GetEntityComponent<Relationship>(childID);

        relationship->RemoveChild(childID);
        childRelationship->RemoveParent();

        rootEntities.push_back(childID);
    }

    /* --- GETTER METHODS --- */

    bool Scene::EntityExists(const EntityID entityID) const noexcept
    {
        return GetEntityComponent<Relationship>(entityID) != nullptr;
    }

    bool Scene::EntityHasParent(const EntityID entityID) const noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot check parent of invalid entity");
            return false;
        }

        return GetEntityComponent<Relationship>(entityID)->GetParentID() != 0;
    }

    EntityID Scene::GetEntityParent(const EntityID entityID) const noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot get parent of invalid entity");
            return EntityID();
        }

        return GetEntityComponent<Relationship>(entityID)->GetParentID();
    }

    std::span<const EntityID> Scene::GetEntityChildren(const EntityID entityID) const noexcept
    {
        if (!EntityExists(entityID))
        {
            APP_WARNING("Cannot get children of invalid entity");
            return { };
        }

        return GetEntityComponent<Relationship>(entityID)->GetChildrenIDs();
    }

}
