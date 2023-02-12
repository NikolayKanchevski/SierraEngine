//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "UUID.h"

#include "../Classes/RNG.h"
#include "../../Core/Rendering/UI/ImGuiUtilities.h"

namespace Sierra::Engine::Components
{
    UUID::UUID()
        : value(Classes::RNG::GetRandomUInt64())
    {

    }

    UUID &UUID::operator=(uint givenValue)
    {
        this->value = givenValue;
        return *this;
    }

    void UUID::OnDrawUI()
    {
        ImGui::BeginProperties();

        ImGui::PushDeactivatedStatus();
        ImGui::UInt64Property("Identifier:", value, nullptr, ImGuiInputTextFlags_ReadOnly);
        ImGui::PopDeactivatedStatus();

        ImGui::EndProperties();
    }

}