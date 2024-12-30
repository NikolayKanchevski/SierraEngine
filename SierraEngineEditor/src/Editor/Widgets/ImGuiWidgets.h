//
// Created by Nikolay Kanchevski on 28.10.24.
//

#pragma once

namespace SierraEngine
{

    struct ImGuiNumericInputInfo
    {
        float32 speed = 1.0f;
        const void* min = nullptr;
        const void* max = nullptr;

        std::string_view format = { };
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;

        ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll;
        float32 rounding = ImGui::GetStyle().FrameRounding;
    };

    struct ImGuiVectorInputInfo
    {
        const void* resetValues = nullptr;

        float32 speed = 1.0f;
        const void* min = nullptr;
        const void* max = nullptr;

        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    };

    struct ImGuiButtonInfo
    {
        ImVec2 size = { 20.0f, 20.0f };
        ImGuiButtonFlags flags = ImGuiButtonFlags_None;

        ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll;
        float32 rounding = ImGui::GetStyle().FrameRounding;
    };

    struct ImGuiDropdownOption
    {
        std::string_view text = "Option";
        bool disabled = false;
    };

    struct ImGuiDropdownInfo
    {
        std::span<const ImGuiDropdownOption> options = { };
        ImGuiComboFlags flags = ImGuiComboFlags_None;
    };

    template<NumericType Numeric>
    consteval ImGuiDataType NumericTypeToImGuiDataType()
    {
        if constexpr (std::is_same<Numeric, int8>())    return ImGuiDataType_S8;
        if constexpr (std::is_same<Numeric, uint8>())   return ImGuiDataType_U8;
        if constexpr (std::is_same<Numeric, int16>())   return ImGuiDataType_S16;
        if constexpr (std::is_same<Numeric, uint16>())  return ImGuiDataType_U16;
        if constexpr (std::is_same<Numeric, int32>())   return ImGuiDataType_S32;
        if constexpr (std::is_same<Numeric, uint32>())  return ImGuiDataType_U32;
        if constexpr (std::is_same<Numeric, int64>())   return ImGuiDataType_S64;
        if constexpr (std::is_same<Numeric, uint64>())  return ImGuiDataType_U64;
        if constexpr (std::is_same<Numeric, float32>()) return ImGuiDataType_Float;
        if constexpr (std::is_same<Numeric, float64>()) return ImGuiDataType_Double;
        return -1;
    }

    namespace ImGuiWidgets
    {
        void PushID(std::string_view ID);
        void PushID(int ID);
        void PopID();

        void BeginHorizontalPadding(float32 padding = ImGui::GetStyle().WindowPadding.x);
        void BeginVerticalPadding(float32 padding = ImGui::GetStyle().WindowPadding.y);
        void BeginPadding(Vector2 padding = ImGui::GetStyle().WindowPadding);
        void EndPadding();

        bool NumericInput(std::string_view label, void* value, ImGuiDataType dataType, const ImGuiNumericInputInfo& inputInfo);
        bool VectorInput(std::string_view label, void* value, size length, ImGuiDataType dataType, const ImGuiVectorInputInfo& info);

        template<NumericType Numeric>
        bool NumericInput(const std::string_view label, Numeric* value, const Numeric min = std::numeric_limits<Numeric>::min(), const Numeric max = std::numeric_limits<Numeric>::max(), const Numeric speed = Numeric(1))
        {
            const ImGuiNumericInputInfo inputInfo
            {
                .speed = static_cast<float32>(speed),
                .min = min != std::numeric_limits<Numeric>::min() ? &min : nullptr,
                .max = min != std::numeric_limits<Numeric>::max() ? &max : nullptr,
                .format = std::is_floating_point_v<Numeric> ? "%.2f" : std::string_view()
            };

            return NumericInput(label, value, NumericTypeToImGuiDataType<Numeric>(), inputInfo);
        }

        template<VectorType Vector>
        bool VectorInput(const std::string_view label, Vector* value, const Vector resetValues = { }, const typename Vector::value_type min = std::numeric_limits<typename Vector::value_type>::min(), const typename Vector::value_type max = std::numeric_limits<typename Vector::value_type>::max(), const typename Vector::value_type speed = typename Vector::value_type(1))
        {
            const ImGuiVectorInputInfo inputInfo
            {
                .resetValues = &resetValues[0],
                .speed = static_cast<float32>(speed),
                .min = min != std::numeric_limits<typename Vector::value_type>::min() ? &min : nullptr,
                .max = min != std::numeric_limits<typename Vector::value_type>::max() ? &max : nullptr
            };

            return VectorInput(label, value, Vector::length(), NumericTypeToImGuiDataType<typename Vector::value_type>(), inputInfo);
        }

        bool Button(std::string_view label, const ImGuiButtonInfo& buttonInfo);
        bool Dropdown(std::string_view label, uint32* value, const ImGuiDropdownInfo& dropdownInfo);
        bool FilePathInput(std::string_view label, const std::filesystem::path& value);

        void BeginPropertyTable(ImGuiTableFlags flags);
        void BeginProperty(std::string_view label, std::string_view tooltip = { });
        void EndProperty();
        void EndPropertyTable();
    }

}
