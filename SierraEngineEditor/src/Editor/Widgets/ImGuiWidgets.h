//
// Created by Nikolay Kanchevski on 28.10.24.
//

#pragma once

namespace SierraEngine
{

    struct ImGuiPropertyTableInfo
    {
        ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_BordersInner | ImGuiTableFlags_PadOuterX;
        ImGuiTableColumnFlags labelColumnFlags = ImGuiTableColumnFlags_None;
        ImGuiTableColumnFlags valueColumnFlags = ImGuiTableColumnFlags_WidthStretch;
    };

    struct ImGuiTextInputInfo
    {
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
        ImGuiInputTextCallback CharacterCallback = nullptr;
        void* userData = nullptr;

        float32 width = ImGui::GetContentRegionAvail().x;
    };

    enum class ImGuiFilePathInputType : bool
    {
        Select,
        Save
    };

    struct ImGuiFileSelectInputInfo
    {
        std::string_view message = { };
        std::string_view buttonText = { };
        const std::filesystem::path* directoryPath = nullptr;

        bool allowFiles = true;
        bool allowDirectories = false;
        std::span<const std::string_view> allowedFileExtensions = { };

        float32 width = ImGui::GetContentRegionAvail().x;
    };

    struct ImGuiFileSaveInputInfo
    {
        std::string_view message = { };
        std::string_view buttonText = { };
        const std::filesystem::path* directoryPath = nullptr;

        std::string_view fileName = { };
        std::span<const std::string_view> allowedFileExtensions = { };

        float32 width = ImGui::GetContentRegionAvail().x;
    };

    struct ImGuiNumericInputInfo
    {
        const void* step = nullptr;
        const void* min = nullptr;
        const void* max = nullptr;
        std::string_view format = { };
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;

        float32 width = ImGui::GetContentRegionAvail().x;
        ImDrawFlags drawFlags = ImDrawFlags_RoundCornersAll;
        float32 rounding = ImGui::GetStyle().FrameRounding;
    };

    struct ImGuiVectorInputInfo
    {
        const void* resetValues = nullptr;
        const void* step = nullptr;
        const void* min = nullptr;
        const void* max = nullptr;
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;

        float32 width = ImGui::GetContentRegionAvail().x;
    };

    struct ImGuiColorInputInfo
    {
        ImGuiColorEditFlags flags = ImGuiColorEditFlags_None;
    };

    enum class ImGuiImageInputFitType : bool
    {
        Fit,
        Stretch
    };

    struct ImGuiImageInputInfo
    {
        std::optional<Sierra::SampledImageID> previewID = std::nullopt;
        ImGuiImageInputFitType fitType = ImGuiImageInputFitType::Fit;

        const std::filesystem::path* directoryPath = nullptr;
        std::span<const std::string_view> allowedFileExtensions = { };

        uint32 imageWidth = 0;
        uint32 imageHeight = 0;

        float32 width = ImGui::GetContentRegionAvail().x;
    };

    struct ImGuiButtonInfo
    {
        ImGuiButtonFlags flags = ImGuiButtonFlags_None;

        float32 width = ImGui::GetContentRegionAvail().x;
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

        float32 width = ImGui::GetContentRegionAvail().x;
    };

    using ImGuiListInputLabelCallback = std::function<std::string(uint32 index)>;

    template<typename T>
    using ImGuiListInputItemCallback = std::function<void(T&, uint32 index)>;

    template<typename T>
    struct ImGuiListInputInfo
    {
        bool allowOperations = true;
        bool opened = false;
        const ImGuiListInputItemCallback<T>& ItemCallback;
        const ImGuiListInputLabelCallback& LabelCallback = [](const uint32 i) -> std::string { return SR_FORMAT("{0}", i); };
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
        void HorizontalIndent(float32 indent = ImGui::GetStyle().IndentSpacing);
        void VerticalIndent(float32 indent = ImGui::GetStyle().IndentSpacing);
        void Indent(Vector2 indent = { ImGui::GetStyle().IndentSpacing, ImGui::GetStyle().IndentSpacing });

        void BeginHorizontalPadding(float32 padding = ImGui::GetStyle().WindowPadding.x);
        void BeginVerticalPadding(float32 padding = ImGui::GetStyle().WindowPadding.y);
        void BeginPadding(Vector2 padding = ImGui::GetStyle().WindowPadding);
        void EndPadding();

        bool BeginPropertyTable(const ImGuiPropertyTableInfo& tableInfo = { });
        void BeginProperty(std::string_view label, std::string_view tooltip = { });
        void EndProperty();
        void EndPropertyTable();

        bool TextInput(std::string_view label, std::string& value, const ImGuiTextInputInfo& inputInfo = { });
        bool FileSelectInput(std::string_view label, std::filesystem::path& value, const Sierra::PlatformContext& platformContext, const ImGuiFileSelectInputInfo& inputInfo = { });
        bool FileSaveInput(std::string_view label, std::filesystem::path& value, const Sierra::PlatformContext& platformContext, const ImGuiFileSaveInputInfo& inputInfo = { });
        bool NumericInput(std::string_view label, void* value, ImGuiDataType dataType, const ImGuiNumericInputInfo& inputInfo = { });
        bool NumericEnterInput(std::string_view label, void* value, ImGuiDataType dataType, const ImGuiNumericInputInfo& inputInfo = { });
        bool VectorInput(std::string_view label, void* value, size length, ImGuiDataType dataType, const ImGuiVectorInputInfo& inputInfo = { });
        bool ColorInput(std::string_view label, ColorRGB& value, const ImGuiColorInputInfo& inputInfo = { });
        bool ColorInput(std::string_view label, ColorRGBA& value, const ImGuiColorInputInfo& inputInfo = { });
        bool ImageInput(std::string_view label, std::filesystem::path& value, const Sierra::PlatformContext& platformContext, const ImGuiImageInputInfo& inputInfo = { });

        bool Button(std::string_view label, const ImGuiButtonInfo& buttonInfo = { });
        bool Dropdown(std::string_view label, uint32& value, const ImGuiDropdownInfo& dropdownInfo);

        template<NumericType Numeric>
        bool NumericInput(const std::string_view label, Numeric& value, const Numeric min = std::numeric_limits<Numeric>::min(), const Numeric max = std::numeric_limits<Numeric>::max(), const Numeric step = Numeric(1))
        {
            const ImGuiNumericInputInfo inputInfo
            {
                .step = &step,
                .min = min != std::numeric_limits<Numeric>::min() ? &min : nullptr,
                .max = min != std::numeric_limits<Numeric>::max() ? &max : nullptr
            };

            return NumericInput(label, &value, NumericTypeToImGuiDataType<Numeric>(), inputInfo);
        }

        template<NumericType Numeric>
        bool NumericEnterInput(const std::string_view label, Numeric& value, const Numeric min = std::numeric_limits<Numeric>::min(), const Numeric max = std::numeric_limits<Numeric>::max(), const Numeric step = Numeric(1))
        {
            const ImGuiNumericInputInfo inputInfo
            {
                .step = &step,
                .min = min != std::numeric_limits<Numeric>::min() ? &min : nullptr,
                .max = min != std::numeric_limits<Numeric>::max() ? &max : nullptr
            };

            return NumericEnterInput(label, &value, NumericTypeToImGuiDataType<Numeric>(), inputInfo);
        }

        template<VectorType Vector>
        bool VectorInput(const std::string_view label, Vector* value, const Vector resetValues = { }, const typename Vector::value_type min = std::numeric_limits<typename Vector::value_type>::min(), const typename Vector::value_type max = std::numeric_limits<typename Vector::value_type>::max(), const typename Vector::value_type step = typename Vector::value_type(1))
        {
            const ImGuiVectorInputInfo inputInfo
            {
                .resetValues = &resetValues[0],
                .step = &step,
                .min = min != std::numeric_limits<typename Vector::value_type>::min() ? &min : nullptr,
                .max = min != std::numeric_limits<typename Vector::value_type>::max() ? &max : nullptr
            };

            return VectorInput(label, value, Vector::length(), NumericTypeToImGuiDataType<typename Vector::value_type>(), inputInfo);
        }

        template<typename T>
        void ListInput(const std::string_view label, std::vector<T>& values, const ImGuiListInputInfo<T>& inputInfo)
        {
            const ImGuiID ID = ImGui::GetID(&values);

            ImGuiStorage& storage = *ImGui::GetStateStorage();
            const std::function Popup = [ID, &storage, &values, &inputInfo]() -> void
            {
                if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) ImGui::OpenPopup(ID);
                if (ImGui::BeginPopupEx(ID, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar))
                {
                    if (ImGui::MenuItem("Add Item", nullptr, false, inputInfo.allowOperations))
                    {
                        if constexpr (std::is_default_constructible_v<T>)
                        {
                            values.emplace_back();
                        }
                        storage.SetBool(ID, true);
                    }
                    if (ImGui::MenuItem("Pop Item", nullptr, false, inputInfo.allowOperations && !values.empty()))
                    {
                        values.pop_back();
                        if (values.empty()) storage.SetBool(ID, false);
                    }
                    if (ImGui::MenuItem("Clear", nullptr, false, inputInfo.allowOperations && !values.empty()))
                    {
                        values.clear();
                        storage.SetBool(ID, false);
                    }
                    ImGui::EndPopup();
                }
            };

            const ImVec4 frameColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
            ImGui::PushStyleColor(ImGuiCol_Header,       frameColor);
            ImGui::PushStyleColor(ImGuiCol_HeaderActive, frameColor);

            ImGui::SetNextItemOpen(storage.GetBool(ID, false));
            ImGui::SetNextItemOpen(inputInfo.opened, ImGuiCond_FirstUseEver);

            const std::string sizeLabel = SR_FORMAT("Size: {0}", values.size(), label);
            if (ImGui::TreeNodeBehavior(ID, ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding, sizeLabel.c_str(), sizeLabel.c_str() + sizeLabel.size()))
            {
                Popup();

                constexpr ImGuiPropertyTableInfo TABLE_INFO =
                {
                    .tableFlags = ImGuiTableFlags_BordersInner | ImGuiTableFlags_SizingFixedFit,
                    .valueColumnFlags = ImGuiTableColumnFlags_WidthStretch
                };

                if (BeginPropertyTable(TABLE_INFO))
                {
                    for (uint32 i = 0; i < values.size(); i++)
                    {
                        BeginProperty(inputInfo.LabelCallback(i));
                        {
                            inputInfo.ItemCallback(values[i], i);
                        }
                        EndProperty();
                    }

                    EndPropertyTable();
                }

                ImGui::TreePop();
            }
            else
            {
                Popup();
            }

            ImGui::PopStyleColor(2);
        }
    }

}
