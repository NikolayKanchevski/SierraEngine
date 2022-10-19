//
// Created by Nikolay Kanchevski on 19.10.22.
//


#include "Application.h"

void Application::DisplayUI(VulkanRenderer &renderer)
{
    // Hierarchy
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNav | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoSavedSettings;

    ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_FirstUseEver, { 0, 0 });
    ImGui::SetNextWindowSize({ (float) renderer.GetWindow().GetWidth() / 6, (float) renderer.GetWindow().GetHeight() }, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints({ 100.0f, 100.0f }, { (float) renderer.GetWindow().GetWidth(), (float) renderer.GetWindow().GetHeight() });
    ImGui::SetNextWindowBgAlpha(0.6f);

    bool hierachyOpen;
    if (ImGui::Begin("Hierarchy", &hierachyOpen, windowFlags) || hierachyOpen)
    {
        ImGui::Separator();

        auto enttRelationshipView = World::GetEnttRegistry().view<Relationship>();
        for (uint32_t i = enttRelationshipView.size(); i--;)
        {
            Relationship &entityRelationship = World::GetEnttRegistry().get<Relationship>(enttRelationshipView[i]);
            if (entityRelationship.GetEnttParentEntity() == entt::null)
            {
                ListDeeper(entityRelationship, 0);
            }
        }

        ImGui::End();
    }

    windowFlags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing |
                  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowPos({ renderer.GetWindow().GetWidth() - 10.0f, 10.0f }, ImGuiCond_Always, { 1, 0 });
    ImGui::SetNextWindowSizeConstraints({ (float) renderer.GetWindow().GetWidth() / 8, (float) renderer.GetWindow().GetHeight() / 8 }, { 10000, 10000 });
    ImGui::SetNextWindowBgAlpha(0.6f);

    // Draw renderer information tab
    bool rendererInfoOpen = true;
    if (ImGui::Begin("Renderer Information", &rendererInfoOpen, windowFlags) || rendererInfoOpen)
    {
        ImGui::Text("%s", ("CPU Frame Time: " + std::to_string(Time::GetFPS()) + " FPS").c_str());
        ImGui::Text("%s", ("GPU Draw Time: " + std::to_string(renderer.GetRendererInfo().drawTime) + " ms").c_str());
        ImGui::Separator();
        ImGui::Text("%s", ("Total meshes being drawn: " + std::to_string(renderer.GetRendererInfo().meshesDrawn)).c_str());
        ImGui::Text("%s", ("Total vertices in scene: " + std::to_string(renderer.GetRendererInfo().verticesDrawn)).c_str());

        ImGui::End();
    }

    // If game pads are present display their info
    for (int i = Input::MAX_GAME_PADS; i--;)
    {
        if (Input::GetGamePadConnected(i))
        {
            bool gamePadInfoOpen = true;
            if (ImGui::Begin((("Game Pad [" + std::to_string(i) + "] Data").c_str()) , &gamePadInfoOpen, ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::Text("%s", ("Game pad [" + Input::GetGamePadName(i) + "] properties:").c_str());
                ImGui::Text("%s", ("Left gamepad stick: [" + std::to_string(Input::GetGamePadLeftStickAxis(i).x) + " || " + std::to_string(Input::GetGamePadLeftStickAxis(i).y) + "]").c_str());
                ImGui::Text("%s", ("Right gamepad stick: [" + std::to_string(Input::GetGamePadRightStickAxis(i).x) + " || " + std::to_string(Input::GetGamePadRightStickAxis(i).y) + "]").c_str());
                ImGui::Text("%s", ("Left trigger: [" + std::to_string(Input::GetGamePadLeftTriggerAxis(i)) + "]").c_str());
                ImGui::Text("%s", ("Right trigger: [" + std::to_string(Input::GetGamePadRightTriggerAxis(i)) + "]").c_str());
                ImGui::RadioButton("\"A\" pressed", Input::GetGamePadButtonPressed(GLFW_GAMEPAD_BUTTON_A, i));
                ImGui::RadioButton("\"A\" held", Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A, i));
                ImGui::RadioButton("\"A\" released", Input::GetGamePadButtonReleased(GLFW_GAMEPAD_BUTTON_A, i));

                ImGui::End();
            }
        }
    }
}

void Application::ListDeeper(Relationship &relationship, const uint32_t iteration)
{
    ImGuiTreeNodeFlags treeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx(relationship.GetComponent<Tag>().tag.c_str(), treeNodeFlag))
    {
        for (auto child : relationship.GetEnttChildrenEntities())
        {
            Relationship &childRelationship = World::GetEnttRegistry().get<Relationship>(child);
            ListDeeper(childRelationship, iteration + 1);
        }

        ImGui::TreePop();
    }

    if (iteration == 0) ImGui::Separator();
}