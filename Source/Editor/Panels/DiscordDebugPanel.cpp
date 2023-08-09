//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "DiscordDebugPanel.h"

#include "../GUI.h"
#include "../../Engine/Classes/Discord.h"

namespace Sierra::Editor
{
    using namespace Engine;

    /* --- POLLING METHODS --- */

    void DiscordDebugPanel::DrawUI()
    {
        if (!Discord::IsInitialized() || !Discord::GetUser().IsLoaded() || !Discord::GetUser().IsIconTextureLoaded()) return;

        if (GUI::BeginWindow("Discord Debugger", WindowFlags::ALWAYS_AUTO_RESIZE | WindowFlags::NO_FOCUS_ON_APPEARING))
        {
            GUI::PushBoldFont();
            GUI::Text("%s", "Detected User:");
            GUI::PopFont();

            GUI::ContinueOnSameLine();
            GUI::Text("%s#%s", Discord::GetUser().GetUsername(), Discord::GetUser().GetDiscriminator());

            GUI::Texture(Discord::GetUser().GetIconTexture()->GetImGuiTextureID(), {
                    GUI::GetRemainingHorizontalSpace() / 3.5f,
                                                                                     GUI::GetRemainingHorizontalSpace() / 3.5f });
            GUI::VerticalIndent(5.0f);

            GUI::PushBoldFont();
            GUI::Text("User Data:");
            GUI::PopFont();

            GUI::Text("Username: %s", Discord::GetUser().GetUsername());
            GUI::Text("Discriminator: %s", Discord::GetUser().GetDiscriminator());
            GUI::Text("ID: %llu", Discord::GetUser().GetId());

        }
        GUI::EndWindow();
    }

}