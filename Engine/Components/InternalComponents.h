//
// Created by Nikolay Kanchevski on 18.10.22.
//

#pragma once



namespace Sierra::Engine::Components
{

    class Tag : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Tag(std::string givenTag) : tag(std::move(givenTag)) { }

        /* --- PROPERTIES --- */
        std::string tag;

        Tag() = default;
        Tag(const Tag&) = default;
    };

    class Relationship : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline explicit Relationship(entt::entity givenSelf) : self(givenSelf) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline entt::entity& GetEnttParentEntity() { return parent; }
        [[nodiscard]] inline std::vector<entt::entity> GetEnttChildrenEntities() const { return children; }

        /* --- SETTER METHODS --- */
        inline void SetParent(entt::entity givenParent)
        {
            // Get the new parent's relationship
            Relationship &givenParentRelationship = World::GetEnttRegistry().get<Relationship>(givenParent);

            // Check if the current parent is not null
            if (parent != entt::null)
            {
                // Get the current parent's relationship
                Relationship &currentParentRelationship = Core::World::GetEnttRegistry().get<Relationship>(parent);

                // Get the index of "self" in current parent's children
                uint32_t childIndex = 0;
                while (currentParentRelationship.children[childIndex] != self)
                {
                    childIndex++;
                }

                // Erase "self" from current parent's children
                currentParentRelationship.children.erase(currentParentRelationship.children.begin() + childIndex);
            }

            // Change parent
            parent = givenParent;

            // Add "self" to new parent's children
            givenParentRelationship.children.push_back(self);
        }

        Relationship() = default;
        Relationship(const Relationship&) = default;

    private:
        entt::entity parent = entt::null;
        entt::entity self = entt::null;
        std::vector<entt::entity> children {};
    };

    class Transform : public Component
    {
    public:
        /* --- PROPERTIES --- */
        glm::vec3 position = {0, 0, 0 };
        glm::vec3 rotation = { 0, 0, 0 };
        glm::vec3 scale = { 1, 1, 1 };

        Transform() = default;
        Transform(const Transform&) = default;
    };
}