//
// Created by Nikolay Kanchevski on 15.05.24.
//

#pragma once

#include "../Component.h"

namespace SierraEngine
{

    class SIERRA_ENGINE_API Relationship final : public Component
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Relationship(entt::entity parent = entt::null);

        /* --- SETTER METHODS --- */
        void SetParent(entt::entity parent);
        void RemoveParent();

        void AddChild(entt::entity child);
        void RemoveChild(entt::entity child);

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool HasParent() const { return parent != entt::null; }
        [[nodiscard]] entt::entity GetParent() const { return parent; }

        [[nodiscard]] uint32 GetChildCount() const { return static_cast<uint32>(children.size()); }
        [[nodiscard]] std::span<const entt::entity> GetChildren() const { return children; }

        /* --- TYPE DATA --- */
        [[nodiscard]] constexpr static std::string_view GetName() { return "Relationship"; }
        [[nodiscard]] constexpr static Signature GetSignature() { return { 'R', 'L', 'S', 'P' }; }
        
        /* --- DESTRUCTOR --- */
        ~Relationship() = default;
    
    private:
        entt::entity parent = entt::null;
        std::vector<entt::entity> children = { };

    };

}
