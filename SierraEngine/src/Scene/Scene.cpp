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

        // NOTE: These functions are to reserve the hash & index of 0 as null

        [[nodiscard]] entt::entity EntityIDToEntity(const EntityID entityID)
        {
            return entt::entity(entityID.GetHash() - 1);
        }

        [[nodiscard]] EntityID EntityToEntityID(const entt::entity entity)
        {
            return EntityID(static_cast<ENTT_ID_TYPE>(entity) + 1);
        }

    }

    /* --- CONSTRUCTORS --- */

    Scene::Scene(const SceneCreateInfo& createInfo)
        : name(createInfo.name)
    {

    }

    /* --- POLLING METHODS --- */

    EntityID Scene::CreateEntity(const std::string_view tag)
    {
        const EntityID entityID = EntityToEntityID(registry.create());
        AddEntityComponent<Tag>(entityID, tag);
        AddEntityComponent<Relationship>(entityID);

        rootEntities.push_back(entityID);
        return entityID;
    }

    void Scene::DestroyEntity(const EntityID entityID)
    {
        RemoveEntityParent(entityID);
        for (const EntityID childID : GetEntityChildren(entityID))
        {
            RemoveEntityChild(entityID, childID);
        }

        registry.destroy(EntityIDToEntity(entityID));
    }

    /* --- SETTER METHODS --- */

    void Scene::SetEntityParent(const EntityID entityID, const EntityID parentID)
    {
        Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return;

        relationship->SetParent(parentID);
        if (Relationship* parentRelationship = GetEntityComponent<Relationship>(relationship->GetParentID()); parentRelationship != nullptr)
        {
            parentRelationship->RemoveChild(entityID);
            parentRelationship->AddChild(entityID);
        }

        const auto iterator = std::ranges::find(rootEntities, entityID);
        if (iterator != rootEntities.end()) rootEntities.erase(iterator);
    }

    void Scene::RemoveEntityParent(const EntityID entityID)
    {
        Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return;

        Relationship* parentRelationship = GetEntityComponent<Relationship>(relationship->GetParentID());
        if (parentRelationship == nullptr) return;

        relationship->RemoveParent();
        parentRelationship->RemoveChild(entityID);

        rootEntities.push_back(entityID);
    }

    void Scene::AddEntityChild(const EntityID entityID, const EntityID childID)
    {
        Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return;

        Relationship* childRelationship = GetEntityComponent<Relationship>(childID);
        if (childRelationship == nullptr) return;

        if (Relationship* parentRelationship = GetEntityComponent<Relationship>(childRelationship->GetParentID()); parentRelationship != nullptr)
        {
            parentRelationship->RemoveChild(childID);
        }
        else
        {
            const auto iterator = std::ranges::find(rootEntities, entityID);
            if (iterator != rootEntities.end()) rootEntities.erase(iterator);
        }

        relationship->AddChild(childID);
        childRelationship->SetParent(entityID);
    }

    void Scene::RemoveEntityChild(const EntityID entityID, const EntityID childID)
    {
        Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return;

        Relationship* childRelationship = GetEntityComponent<Relationship>(childID);
        if (childRelationship == nullptr) return;

        relationship->RemoveChild(childID);
        childRelationship->RemoveParent();

        rootEntities.push_back(childID);
    }

    /* --- GETTER METHODS --- */

    bool Scene::EntityExists(const EntityID entityID) const
    {
        return GetEntityComponent<Relationship>(entityID) != nullptr;
    }

    bool Scene::EntityHasParent(const EntityID entityID) const
    {
        const Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return false;
        return relationship->GetParentID() != 0;
    }

    EntityID Scene::GetEntityParent(const EntityID entityID) const
    {
        const Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return { };
        return relationship->GetParentID();
    }

    std::span<const EntityID> Scene::GetEntityChildren(const EntityID entityID) const
    {
        const Relationship* relationship = GetEntityComponent<Relationship>(entityID);
        if (relationship == nullptr) return { };
        return relationship->GetChildrenIDs();
    }

}
