//
// Created by Nikolay Kanchevski on 18.01.23.
//

#include "UUID.h"

#include "../Classes/RNG.h"
#include "../../Editor/GUI.h"

namespace Sierra::Engine
{
    UUID::UUID()
        : value(RNG::GetRandomUInt64())
    {

    }

    void UUID::OnDrawUI()
    {
        using namespace Editor;
        GUI::BeginProperties();

        GUI::PushDeactivatedStatus();
        GUI::UInt64Property("Identifier:", value, nullptr, InputFlags::READ_ONLY);
        GUI::PopDeactivatedStatus();

        GUI::EndProperties();
    }

}