//
// Created by Nikolay Kanchevski on 11.01.23.
//

#pragma once

#include "../Vulkan/Abstractions/Texture.h"
using Sierra::Core::Rendering::Vulkan::Abstractions::Texture;

#define HOVER_TIME_THRESHOLD 1.0f

namespace GUI
{
    void GenerateID();

    void ExternalPushID();
    void ExternalPopID();

    void VerticalIndent(float height);

    void PushDeactivatedStatus();
    void PopDeactivatedStatus();

    void SetNumericFormattingSpaces(uint spacesCount);
    void ResetNumericFormattingSpaces();

    void SetNumericStepChange(float newStep);
    void ResetNumericStepChange();

    void SetInputLimits(Vector2 limits);
    void ResetInputLimits();

    void BoldText(const char* text);
    void CustomLabel(const char* label);
    void ShowTooltip(const char* tooltip);

    bool BeginWindow(const char* title, bool *open = nullptr, ImGuiWindowFlags windowFlags = 0);
    void EndWindow();

    void BeginProperties(ImGuiTableFlags tableFlags = ImGuiTableFlags_None);
    void EndProperties();

    void BeginProperty(const char* label);
    void EndProperty();

    bool BeginTreeProperties(const char* label);
    void EndTreeProperties();

    void PropertyTabHeader(const char* label);

    /* --- RAW INPUTS --- */
    bool StringInput(const char* labelID, String &value, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool IntInput(const char* labelID, int &value, bool canDrag = false, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool Int64Input(const char* labelID, int64 &value, bool canDrag = false, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool UIntInput(const char* labelID, uint &value, bool canDrag = false, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool UInt64Input(const char* labelID, uint64 &value, bool canDrag = false, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool FloatInput(const char* labelID, float &value, bool canDrag = false, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool DoubleInput(const char* labelID, double &value, bool canDrag = false, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool Checkbox(const char* labelID, bool &value);
    bool Vector3Input(Vector3 &value, const float *resetValues, const char** tooltips = nullptr);
    bool Dropdown(const char* labelID, uint &value, const char** options, uint optionsCount, const bool* deactivatedFlags = nullptr);
    bool RoundedButton(const char* label, const ImVec2 &givenSize, ImDrawFlags roundingType, float rounding = GImGui->Style.FrameRounding, ImGuiButtonFlags flags = 0);

    template<typename T>
    inline bool AnyInput(const char* labelID, T &value)
    {
        if constexpr(std::is_same_v<T, String>) { return StringInput(labelID, value); }
        else if constexpr(std::is_same_v<T, int>) { return IntInput(labelID, value); }
        else if constexpr(std::is_same_v<T, int64>) { return Int64Input(labelID, value); }
        else if constexpr(std::is_same_v<T, uint>) { return UIntInput(labelID, value); }
        else if constexpr(std::is_same_v<T, uint64>) { return UInt64Input(labelID, value); }
        else if constexpr(std::is_same_v<T, float>) { return FloatInput(labelID, value); }
        else if constexpr(std::is_same_v<T, double>) { return DoubleInput(labelID, value); }
        else if constexpr(std::is_same_v<T, bool>) { return Checkbox(labelID, value); }
        else if constexpr(std::is_same_v<T, Vector3>) { const float resetValues[3] = { 0.0f, 0.0f, 0.0f }; return Vector3Input(value, resetValues); }
        else return false;
    }

    /* --- PROPERTIES --- */
    bool StringProperty(const char* label, String &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool IntProperty(const char* label, int &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool Int64Property(const char* label, int64 &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool UIntProperty(const char* label, uint &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool UInt64Property(const char* label, uint64 &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool FloatProperty(const char* label, float &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool DoubleProperty(const char* label, double &value, const char* tooltip = nullptr, ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_None);
    bool CheckboxProperty(const char* label, bool &value, const char* tooltip = nullptr);
    bool DropdownProperty(const char* label, uint &value, const char** options, uint optionsCount, const bool* deactivatedFlags = nullptr, const char* tooltip = nullptr);
    bool PropertyVector3(const char* label, Vector3 &value, const float *resetValues, const char** tooltips = nullptr);
    bool TextureProperty(const char* label, SharedPtr<Texture> &texture, const char* tooltip = nullptr);

    template<typename T>
    inline bool AnyPropertyInput(const char* labelID, T &value)
    {
        if constexpr(std::is_same_v<T, String>) { return StringProperty(labelID, value); }
        else if constexpr(std::is_same_v<T, int>) { return IntProperty(labelID, value); }
        else if constexpr(std::is_same_v<T, int64>) { return Int64Property(labelID, value); }
        else if constexpr(std::is_same_v<T, uint>) { return UIntProperty(labelID, value); }
        else if constexpr(std::is_same_v<T, uint64>) { return UInt64Property(labelID, value); }
        else if constexpr(std::is_same_v<T, float>) { return FloatProperty(labelID, value); }
        else if constexpr(std::is_same_v<T, double>) { return DoubleProperty(labelID, value); }
        else if constexpr(std::is_same_v<T, bool>) { return CheckboxProperty(labelID, value); }
        else if constexpr(std::is_same_v<T, Vector3>) { const float resetValues[3] = { 0.0f, 0.0f, 0.0f }; return PropertyVector3(labelID, value, resetValues); }
        else return false;
    }

    template<typename T>
    inline void DrawComponent(entt::entity entity)
    {
        using namespace Sierra::Core;

        const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding;
        if (World::HasComponent<T>(entity))
        {
            auto& component = World::GetComponent<T>(entity);
            ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

            bool open = ImGui::TreeNodeEx((void*)(typeid(T).hash_code() << static_cast<uint32_t>(entity)), treeNodeFlags, "%s", Debugger::TypeToString<T>().c_str());
            ImGui::PopStyleVar();

            ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
            if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
            {
                ImGui::OpenPopup("ComponentSettings");
            }

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Remove component"))
                {
                    removeComponent = true;
                }

                ImGui::EndPopup();
            }

            if (open)
            {
                component.OnDrawUI();
                GUI::VerticalIndent(GImGui->Style.ItemSpacing.y);
                ImGui::TreePop();
            }

            if (removeComponent)
            {
//                World::RemoveComponent<T>(entity);
            }
        }
    }

    template<typename T>
    inline bool AutoDrawFields(T &reference)
    {
        GUI::BeginProperties();

        bool modified = false;
        T::Class::ForEachField(reference, [&modified](auto & field, auto & value){
            using Type = typename std::remove_reference<decltype(value)>::type;
            std::string stringName = std::string(field.name);
            stringName[0] = std::toupper(stringName[0]);
            modified = modified || GUI::AnyPropertyInput<Type>(stringName.c_str(), value);
        });

        GUI::EndProperties();

        return modified;
    }
}
