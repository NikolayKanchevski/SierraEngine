//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "Relationship.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    void Relationship::OnAddComponent()
    {
        World::GetOriginEntities().push_back(GetEntity().GetEnttEntity());
    }

    /* --- SETTER METHODS --- */

    void Relationship::SetParent(const Entity &newParent)
    {
        // Get the new parent's relationship
        Entity self = GetEntity();
        Relationship &newParentRelationship = newParent.GetComponent<Relationship>();

        // Check if the current parent is not null
        bool usedToBeOrigin = parent.IsNull();
        if (!usedToBeOrigin)
        {
            // Remove "self" from siblings
            auto &siblings = parent.GetComponent<Relationship>().children;
            for (uint32 i = siblings.size(); i--;)
            {
                if (siblings[i] == self)
                {
                    siblings.erase(std::next(siblings.begin(), i));
                    break;
                }
            }
        }

        // Change parent
        parent = newParent;
        newParentRelationship.children.push_back(self);

        auto &originEntities = World::GetOriginEntities();
        if (newParent.IsNull())
        {
            originEntities.push_back(self.GetEnttEntity());
        }
        else
        {
            if (usedToBeOrigin) originEntities.erase(std::remove(originEntities.begin(), originEntities.end(), self.GetEnttEntity()), originEntities.end());
        }
    }

}
