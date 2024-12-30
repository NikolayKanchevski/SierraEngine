//
// Created by Nikolay Kanchevski on 28.10.24.
//

#include "PropertiesPanel.h"

#include "../Widgets/ImGuiWidgets.h"

namespace SierraEngine
{

    namespace
    {
        constexpr ImGuiTableFlags DEFAULT_TABLE_FLAGS = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV;

        template<ComponentType Component>
        void DrawComponentProperties(Component&) { }

        template<> void DrawComponentProperties<Tag>(Tag& tag)
        {
            ImGuiWidgets::BeginProperty("Tag");
            {
                std::string modifiedTag = std::string(tag.GetTag());
                if (ImGui::InputText("##TagInput", &modifiedTag)) tag.SetTag(modifiedTag);
            }
            ImGuiWidgets::EndProperty();
        }

        template<> void DrawComponentProperties<Transform>(Transform& transform)
        {
            ImGuiWidgets::BeginProperty("Position");
            {
                Vector3 modifiedPosition = transform.GetPosition();
                if (ImGuiWidgets::VectorInput<Vector3>("##PositionInput", &modifiedPosition)) transform.SetPosition(modifiedPosition);
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Rotation");
            {
                Vector3 modifiedRotation = transform.GetRotation();
                if (ImGuiWidgets::VectorInput("##RotationInput", &modifiedRotation)) transform.SetRotation(modifiedRotation);
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Scale");
            {
                Vector3 modifiedScale = transform.GetScale();
                if (ImGuiWidgets::VectorInput("##ScaleInput", &modifiedScale, { 1.0f, 1.0f, 1.0f })) transform.SetScale(modifiedScale);
            }
            ImGuiWidgets::EndProperty();
        }

        template<> void DrawComponentProperties<Camera>(Camera& camera)
        {
            ImGuiWidgets::BeginProperty("Projection Type");
            {
                constexpr std::array<ImGuiDropdownOption, 2> PROJECTION_TYPE_OPTIONS
                {
                    ImGuiDropdownOption { .text = "Perspective" },
                    ImGuiDropdownOption { .text = "Orthographic" }
                };

                if (uint32 modifiedProjectionType = static_cast<uint32>(camera.GetProjectionType()); ImGuiWidgets::Dropdown("##ProjectionTypeDropdown", &modifiedProjectionType, { .options = PROJECTION_TYPE_OPTIONS }))
                {
                    camera.SetProjectionType(static_cast<ProjectionType>(modifiedProjectionType));
                }
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Near Clip");
            {
                float32 modifiedNearClip = camera.GetNearClip();
                if (ImGuiWidgets::NumericInput("##NearClip", &modifiedNearClip, 0.01f, camera.GetFarClip())) camera.SetNearClip(modifiedNearClip);
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Far Clip");
            {
                float32 modifiedFarClip = camera.GetFarClip();
                if (ImGuiWidgets::NumericInput("##FarClipInput", &modifiedFarClip, camera.GetNearClip())) camera.SetFarClip(modifiedFarClip);
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Field of View");
            {
                float32 modifiedFieldOfView = camera.GetFieldOfView();
                if (ImGuiWidgets::NumericInput("##FieldOfViewInput", &modifiedFieldOfView, 0.0f, 110.0f)) camera.SetFieldOfView(modifiedFieldOfView);
            }
            ImGuiWidgets::EndProperty();
        }

        template<ComponentType Component>
        void DrawComponent(const EntityID entityID, Scene& scene)
        {
            Component* component = scene.GetEntityComponent<Component>(entityID);
            if (component == nullptr) return;

            const ImGuiStyle style = ImGui::GetStyle();
            ImGuiWidgets::PushID(typeid(Component).hash_code());

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { style.FramePadding.x * 1.5f, style.FramePadding.y * 1.5f });
            constexpr ImGuiTreeNodeFlags TREE_FLAGS = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowOverlap | ImGuiTreeNodeFlags_FramePadding;
            const bool open = ImGui::TreeNodeEx(Component::GetName().data(), TREE_FLAGS);
            ImGui::PopStyleVar();

            constexpr std::string COMPONENT_SETTINGS_ID = "ComponentSettings";
            if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::OpenPopup(COMPONENT_SETTINGS_ID.data());

            bool removeComponent = false;
            if (ImGui::BeginPopup(COMPONENT_SETTINGS_ID.data()))
            {
                if constexpr (RequiredComponents::Contains<Component>) ImGui::BeginDisabled();
                removeComponent = ImGui::MenuItem("Remove component");
                if constexpr (RequiredComponents::Contains<Component>) ImGui::EndDisabled();

                ImGui::EndPopup();
            }

            if (removeComponent)
            {
                scene.RemoveEntityComponent<Component>(entityID);
            }
            else if (open)
            {
                ImGuiWidgets::BeginPropertyTable(DEFAULT_TABLE_FLAGS);
                DrawComponentProperties(*component);
                ImGuiWidgets::EndPropertyTable();
            }

            ImGuiWidgets::PopID();
        }

        template<ComponentType... Components>
        void DrawComponents(const EntityID entityID, Scene& scene, const ComponentGroup<Components...>)
        {
            ([&]() -> void
            {
                DrawComponent<Components>(entityID, scene);
            }(), ...);
        }
    }

    /* --- POLLING METHODS --- */

    void PropertiesPanel::Draw(const std::optional<EntityID> entityID, Scene& scene)
    {
        if (ImGui::Begin("Properties", nullptr, DEFAULT_WINDOW_FLAGS))
        {
            if (entityID.has_value())
            {
                DrawComponents(*entityID, scene, AllComponents());
            }
        }
        ImGui::End();
    }

}