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

    UUID &UUID::operator=(const uint givenValue)
    {
        value = givenValue;
        return *this;
    }

    void UUID::OnDrawUI()
    {
        GUI::BeginProperties();

        GUI::PushDeactivatedStatus();
        GUI::UInt64Property("Identifier:", value, nullptr, ImGuiInputTextFlags_ReadOnly);
        GUI::PopDeactivatedStatus();

        GUI::EndProperties();
    }

}